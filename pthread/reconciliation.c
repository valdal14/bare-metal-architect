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
#define COMPLETION_MASK 0x0B

typedef enum
{
    DS,
    PD 
} PipelineType;

typedef struct 
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
 * @brief Inits a DataSource
 * @param DataSource datasource double pointer
 * @param PipelineType type
 * @param char ds_code
 * @return void
 */
void init_datasource(DataSource **datasource, PipelineType type, char code[DS_CODE])
{
    uint8_t ds_code_size = strlen(code) + 1;

    if(ds_code_size != 6)
    {
        fprintf(stderr, "DataSource code must be at most 5 chars length\n");
        exit(EXIT_FAILURE);
    }

    DataSource *ds = (DataSource *)calloc(1, sizeof(DataSource));
    check_allocation(ds, type);
    
    strncpy(ds->ds_code, code, DS_CODE);
    ds->ds_code[ds_code_size] = '\0';
    ds->head = NULL;
    ds->tail = NULL;
    ds->size = 0;
    ds->capacity = DS_MAX_SIZE;

    *datasource = ds;
}

int main(void)
{
    DataSource *snowflake = NULL;
    DataSource *anaplan = NULL;
    init_datasource(&snowflake, DS, "SNOWF");
    init_datasource(&anaplan, DS, "ANAPL");
    printf("DataSource %s Allocated at address %p\n", snowflake->ds_code, snowflake);
    printf("DataSource %s Allocated at address %p\n", anaplan->ds_code, anaplan);

    return 0;
}
