#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define COST_CENTER_CODE 7
#define DS_MAX_SIZE 10
#define DS_CODE 6
// Bitwise Macros
#define BIT(x) (1 << (x))
#define DEFAULT_MASK 0x00
#define SOURCE_BIT 0 
#define DEST_BIT 1 
#define PROCESS_BIT 3 
#define RECONCILIATION_READY_BIT 7
#define ANAPLAN_META_MASK 0x09
#define SNOWFLAKE_META_MASK 0x0A 
// DSs 
#define SNOWFLAKE "SNOWF"
#define ANAPLAN "ANAPL"

typedef enum
{
    DS,
    PD 
} PipelineType;

typedef struct PipelineData
{
    /**
     * 8 bytes
     * 7 byte
     * 1 byte
     * -------
     * 16 bytes + 0 paddings 
     */
    struct PipelineData *next;
    char cc_code[COST_CENTER_CODE];
    /**
     * bit 0 source 
     * bit 2 destination
     * bit 4 processed
     * bit 8 reconciliation
     * I.E:  
     * SNOWFLAKE source 0 - destination 1 
     * ANAPLAN   source 1 - destination 0 
     */
    uint8_t metadata;

} PipelineData;

typedef struct
{
    /**
     * 8 bytes 
     * 8 bytes 
     * 6 bytes
     * 1 byte
     * 1 byte
     * -------
     * 24 bytes + 0 paddings 
     */
    struct PipelineData *head;
    struct PipelineData *tail;
    char ds_code[DS_CODE];
    uint8_t size;
    uint8_t capacity;
    
} DataSource;

/**
 * @brief Checks the allocation of a given PipelineType
 * @param void pointer
 * @param PipelineType type
 * @return void
 */
void check_allocation(void *type, PipelineType p_type)
{
    switch(p_type)
    {
        case DS:
            if((DataSource *)type == NULL)
            {
                fprintf(stderr, "Casting DataSource type failed\n");
                exit(EXIT_FAILURE);
            }
            break;
        case PD:
            if((PipelineData *)type == NULL)
            {
                fprintf(stderr, "Casting PipelineData type failed\n");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            fprintf(stderr, "Unsupported Type Provided\n");
            exit(EXIT_FAILURE);
    }
}

/**
 * @brief Validates the DataSource Code Identifier and 
 * return the metadata value associated with it 
 * @param char ds_code 
 * @return uint8_t
 */
uint8_t validate_ds_code(char ds_code[DS_CODE])
{
    if(strcmp(ds_code, SNOWFLAKE) == 0) return 0;
    if(strcmp(ds_code, ANAPLAN) == 0) return 1;
    fprintf(stderr, "Invalid DataSource Code Identifier\n");
    exit(EXIT_FAILURE);
}

/**
 * @brief Inits a DataSource
 * @param DataSource datasource double pointer
 * @param PipelineType type
 * @param char ds_code
 * @return void
 */
void init_datasource(DataSource **datasource, PipelineType type, char code[DS_CODE])
{
    validate_ds_code(code);

    DataSource *ds = (DataSource *)calloc(1, sizeof(DataSource));
    check_allocation(ds, type);
    
    strncpy(ds->ds_code, code, DS_CODE);
    ds->ds_code[strlen(code) + 1] = '\0';
    ds->head = NULL;
    ds->tail = NULL;
    ds->size = 0;
    ds->capacity = DS_MAX_SIZE;

    *datasource = ds;
}

/**
 * @brief Adds the metadata to the PipelineData
 * @param PipelineData pd pointer
 * @param uint8_t ds_code (DataSource source metadata code)
 * @return void
 */
void set_metadata(PipelineData *pd, uint8_t ds_code)
{
    pd->metadata = DEFAULT_MASK;
    pd->metadata |= BIT(PROCESS_BIT);

    // SNOWFLAKE Source - Valid Value = 1010
    if(ds_code == 0)
        pd->metadata |= SNOWFLAKE_META_MASK;
    // ANAPLAN Source   - Valid Value = 1001 
    else if(ds_code == 1)
        pd->metadata |= ANAPLAN_META_MASK;
    else
    {
        fprintf(stderr, "Invalid DataSource Source Code\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Adds a new PipelineData to the DataSource  
 * @param DataSource ds pointer
 * @param char cc_code
 * @param PipelineType type
 * @param void(*handle_metadata)(PipelineData *pd, uint8_t source_code)
 * @return void
 */
void add_data(DataSource *ds, char cc_code[COST_CENTER_CODE], PipelineType type, void(*handle_metadata)(PipelineData *pd, uint8_t source_code))
{
    if(ds->size + 1 > ds->capacity)
    {
        fprintf(stderr, "Max Data Packet Capacity Reached\n");
        exit(EXIT_FAILURE);
    }

    uint8_t cc_code_size = strlen(cc_code) + 1;

    if(cc_code_size != COST_CENTER_CODE)
    {
        fprintf(stderr, "Cost Center code must be 6 chars length\n");
        exit(EXIT_FAILURE);
    }

    // allocate space for the new PipelineData node
    PipelineData *pd = (PipelineData *)malloc(sizeof(PipelineData));
    check_allocation(pd, type);
    // copy the cc_code
    strncpy(pd->cc_code, cc_code, cc_code_size);
    pd->cc_code[cc_code_size] = '\0';
    // Set PipelineData Metadata
    uint8_t ds_meta_source = validate_ds_code(ds->ds_code);
    handle_metadata(pd, ds_meta_source);    
    // Set the next PipelineData node to NULL
    pd->next = NULL;

    // Insertion 
    if(ds->head == NULL)
    {
        ds->head = pd;
        ds->tail = pd;
    }
    else 
    {
        ds->tail->next = pd;
        ds->tail = pd;
    }

    // Increase the DS size 
    ds->size += 1;
}

/**
 * @brief Prints the PipelineData stored in the DataSource
 * @param DataSource ds pointer
 * @return void
 */
void print_ds_data(DataSource *ds)
{
    PipelineData *current = ds->head;
    if(current == NULL) return;

    printf("DataSource [%s] current size = %d\n", ds->ds_code, ds->size);
    printf("-----------------------------------\n");
    
    while(current != NULL)
    {
        printf("CCC: %s\n", current->cc_code);
        
        if((current->metadata & BIT(SOURCE_BIT)) == 0)
        {
            printf("Source      : %s\n", SNOWFLAKE);
            printf("Destination : %s\n", ANAPLAN);
        }
        else 
        {
            printf("Source      : %s\n", ANAPLAN);
            printf("Destination : %s\n", SNOWFLAKE);
        }
        
        current = current->next;
        printf("-----------------------------------\n");
    }
}

/**
 * @brief Removes all allocated objects
 * @param DataSource ds pointer
 * @return void
 */
void clean(DataSource *ds)
{
    PipelineData *current = ds->head;
    if(current == NULL) return;

    PipelineData *next_node = NULL;

    while(current != NULL)
    {
        next_node = current->next;
        free(current);
        current = next_node;
    }

    free(ds);
    ds = NULL;
}

int main(void)
{
    DataSource *snowflake = NULL;
    DataSource *anaplan = NULL;
    init_datasource(&snowflake, DS, SNOWFLAKE);
    init_datasource(&anaplan, DS, ANAPLAN);
    printf("DataSource %s Allocated at address %p\n", snowflake->ds_code, snowflake);
    printf("DataSource %s Allocated at address %p\n", anaplan->ds_code, anaplan);
    add_data(snowflake, "CCIT01", PD, set_metadata);
    add_data(snowflake, "CCIT02", PD, set_metadata);
    add_data(snowflake, "CCIT03", PD, set_metadata);
    print_ds_data(snowflake); 
    

    clean(snowflake);
    clean(anaplan);
    return 0;
}
