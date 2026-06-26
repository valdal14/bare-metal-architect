#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#define TOGGLE(x) (1 << (x))
#define DEFAULT_STATE_MASK 0x00
#define P_STATE_BIT 0
#define P_R_BIT 1
#define P_W_BIT 3

#define BUFFER_SIZE 8
pthread_mutex_t mutex;
pthread_cond_t not_empty;
pthread_cond_t not_full;


typedef struct
{
    uint8_t buffer[BUFFER_SIZE];
    uint8_t count;
    uint8_t head;
    uint8_t tail;
} Telemetry;

typedef struct
{
    pthread_t read;
    pthread_t store;
    /*
     * bit 0 = on/off state  
     * bit 2 = read 
     * bit 4 = store 
     */
    uint8_t states;
} TelemetryProcess;

/**
 * @brief Helper method to check the Telemetry allocation 
 * @param Telemetry telemetry pointer
 * @return void
 */
void validate_telemetry(Telemetry *telemetry)
{
    if(telemetry == NULL)
    {
        fprintf(stderr, "Could not allocate space for the Telemetry\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Init the Telemetry Data Structure
 * @param Telemetry telemetry double pointer
 * @return void
 */
void init_telemetry(Telemetry **telemetry)
{
    Telemetry *t = (Telemetry *)calloc(1, sizeof(Telemetry));
    validate_telemetry(t);

    t->count = 0;
    t->head = 0;
    t->tail = 0;

    *telemetry = t;
}

/**
 * @brief Callback of the pthread_create for the thread 
 * responsible of reading the telemetry data 
 * @param void arg pointer
 * @return void pointer
 */
void *read_sensor(void *arg)
{
    Telemetry *telemetry = (Telemetry *)arg;
    validate_telemetry(telemetry);

    return NULL;
}

/**
 * @brief Callback of the pthread_create for the thread 
 * responsible of storing the telemetry data 
 * @param void arg pointer
 * @return void pointer
 */
void *store_sensor_data(void *arg)
{
    Telemetry *telemetry = (Telemetry *)arg;
    validate_telemetry(telemetry);

    return NULL;
}

/**
 * @brief Inits the mutex and POSIX thread conditions
 * @return void
 */
void init_concurrency_model(void)
{
    pthread_cond_init(&not_empty, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_mutex_init(&mutex, NULL); 
}

/**
 * @brief Cleans up the allocated objects, the POSIX
 * conditions and the mutex 
 * @param Telemetry telemetry pointer
 * @return void
 */
void clean_up(Telemetry *telemetry)
{
    free(telemetry);
    pthread_cond_destroy(&not_empty);
    pthread_cond_destroy(&not_full);
    pthread_mutex_destroy(&mutex);
}

int main(void)
{
    TelemetryProcess process;
    Telemetry *telemetry = NULL;
    init_concurrency_model();
    init_telemetry(&telemetry);
    printf("Telemetry allocated at address %p\n", telemetry);



    clean_up(telemetry);

    return 0;
}
