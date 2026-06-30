#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define DS_MAX_SIZE 100
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
// Concurrency Primitives
pthread_cond_t ready = PTHREAD_COND_INITIALIZER;
pthread_cond_t process_status = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// Global state 
uint8_t reconciliate_state = 0;
uint8_t process_reco = 0;

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
    uint8_t cc_code;
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

typedef struct DataSource
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
    uint32_t size;
    uint32_t capacity;
} DataSource;

typedef struct Extractor
{
    struct DataSource *ds;
    uint8_t quantity;
} Extractor;

typedef struct ReconciliateDS
{
    Extractor *ds1;
    Extractor *ds2;

} ReconciliateDS;

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
 * @param uint8_t cc_code
 * @param PipelineType type
 * @param void(*handle_metadata)(PipelineData *pd, uint8_t source_code)
 * @return void
 */
void add_data(DataSource *ds, uint8_t cc_code, PipelineType type, void(*handle_metadata)(PipelineData *pd, uint8_t source_code))
{
    if(ds->size + 1 > ds->capacity)
    {
        fprintf(stderr, "Max Data Packet Capacity Reached\n");
        exit(EXIT_FAILURE);
    }

    // allocate space for the new PipelineData node
    PipelineData *pd = (PipelineData *)malloc(sizeof(PipelineData));
    check_allocation(pd, type);
    // add the data cc_code
    pd->cc_code = cc_code;
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
 * @brief Simulates the data extraction and processing 
 * @param DataSource ds pointer
 * @param uint8_t size
 * @return void
 */
void extract_and_process(DataSource *ds, uint8_t size)
{
    if(size > DS_MAX_SIZE)
    {
        fprintf(stderr, "The size of the data to extract is greater than the max capacity\n");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&mutex);
    char *ds_code = ds->ds_code;
    pthread_mutex_unlock(&mutex);

    for(int i = 0; i < size; i++)
    {
        printf("Extracting Data From DataSource %s packet: %d\n", ds_code, i);
        sleep(1);
        pthread_mutex_lock(&mutex);
        add_data(ds, i, PD, set_metadata);
        pthread_mutex_unlock(&mutex);
    }

    PipelineData *current = ds->head;
    
    while(current != NULL)
    {
        pthread_mutex_lock(&mutex);
        current->metadata |= BIT(RECONCILIATION_READY_BIT);
        uint8_t metadata_status = current->metadata;
        pthread_mutex_unlock(&mutex);
        current = current->next;

        sleep(1);
        printf("DataSource %s packet code: %d\n", ds_code, metadata_status);
    }

    // set the DataSource ready for reconciliation
    pthread_mutex_lock(&mutex);
    reconciliate_state += 1;
    if(reconciliate_state == 2) pthread_cond_broadcast(&ready);
    pthread_mutex_unlock(&mutex);
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

    pthread_mutex_lock(&mutex);
    printf("\nDataSource [%s] current size = %d\n", ds->ds_code, ds->size);
    printf("-----------------------------------\n");
    pthread_mutex_unlock(&mutex);

    while(current != NULL)
    {
        sleep(1);

        pthread_mutex_lock(&mutex);
        printf("CCC: %d\n", current->cc_code);
        
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
        pthread_mutex_unlock(&mutex);
        printf("-----------------------------------\n");
    }

    pthread_mutex_lock(&mutex);
    process_reco += 1;
    if(process_reco == 2) pthread_cond_broadcast(&process_status);
    pthread_mutex_unlock(&mutex);
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

/**
 * @brief pthread_create callback to extract the data from a given DataSource
 * @param void arg pointer
 * @return void pointer
 */
void *extract(void *arg)
{
    Extractor *ext = (Extractor *)arg;
    
    if(ext == NULL)
    {
        fprintf(stderr, "Could not cast the Extractor\n");
        exit(EXIT_FAILURE);
    }

    extract_and_process(ext->ds, ext->quantity); 

    return (void *)ext;
}

/**
 * @brief pthread_create callback to start the reconciliate process 
 * @param void arg pointer
 * @return void
 */
void *start_reconciliation(void *arg)
{
    ReconciliateDS *reco = (ReconciliateDS *)arg;
    if(reco == NULL) exit(EXIT_FAILURE);
    
    pthread_mutex_lock(&mutex);
    
    while(reconciliate_state != 2)
    {
        pthread_cond_wait(&ready, &mutex);
    }
    
    pthread_mutex_unlock(&mutex);
    
    print_ds_data(reco->ds1->ds);    
    print_ds_data(reco->ds2->ds);    

    return (void *)reco;
}

/**
 * @brief pthread_create callback to verify the status of the reconciliation process
 * @param void arg pointer
 * @return void
 */
void *process_reconciliation(void *arg)
{
    ReconciliateDS *reco = (ReconciliateDS *)arg;
    
    pthread_mutex_lock(&mutex);

    while(process_reco != 2)
    {
        pthread_cond_wait(&process_status, &mutex);
    }

    DataSource *c1 = reco->ds1->ds;
    DataSource *c2 = reco->ds2->ds;
   
    printf("\n--- Reconciliation Status ---\n");

    if(c1->size != c2->size)
    {
        fprintf(stderr, "Reconciliate Failed. %s size = %d while %s size = %d\n", c1->ds_code, c1->size, c2->ds_code, c2->size);
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Reconciliation Process Completed Successfully\n");
        printf("%s and %s DataSources have the same size and the total package is: %d\n", c1->ds_code, c2->ds_code, (c1->size + c2->size));
    }

    pthread_mutex_unlock(&mutex);

    return (void *)reco;

}

int main(void)
{
    pthread_t snow_ext, ana_ext, reconciliate, process;

    DataSource *snowflake = NULL;
    DataSource *anaplan = NULL;
    init_datasource(&snowflake, DS, SNOWFLAKE);
    init_datasource(&anaplan, DS, ANAPLAN);
    printf("DataSource %s Allocated at address %p\n", snowflake->ds_code, snowflake);
    printf("DataSource %s Allocated at address %p\n", anaplan->ds_code, anaplan);
    // Define the extraction process 
    Extractor snow;
    snow.ds = snowflake;
    snow.quantity = 6;

    Extractor ana;
    ana.ds = anaplan;
    ana.quantity = 6;

    ReconciliateDS reco;
    reco.ds1 = &snow;
    reco.ds2 = &ana;

    // Create dedicated, empty pointers to catch the return values
    void *snow_res = NULL;
    void *ana_res = NULL;
    void *reco_res = NULL;
    void *process_reco_res = NULL;

    pthread_create(&snow_ext, NULL, extract, (void *)&snow);
    pthread_create(&ana_ext, NULL, extract, (void *)&ana);
    pthread_create(&reconciliate, NULL, start_reconciliation, (void *)&reco);
    pthread_create(&process, NULL, process_reconciliation, (void *)&reco);

    pthread_join(snow_ext, &snow_res);
    pthread_join(ana_ext, &ana_res);
    pthread_join(reconciliate, &reco_res);
    pthread_join(process, &process_reco_res);

    clean(snowflake);
    clean(anaplan);
    return 0;
}
