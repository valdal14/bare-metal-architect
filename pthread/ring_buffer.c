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

typedef enum
{
    READER,
    WRITER,
    TRACKER
} TelemetryType;

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

typedef struct 
{
    Telemetry *t;
    TelemetryProcess *p;
} F1Tracking;

/**
 * @brief Helper method to check the Telemetry allocation 
 * @param void f1_telemetry pointer
 * @param TelemetryType type
 * @return void
 */
void validate_telemetry(void *f1_telemetry, TelemetryType type)
{
    switch(type)
    {
        case READER:
            if((Telemetry *)f1_telemetry == NULL)
            {
                fprintf(stderr, "Could not allocate space for the Telemetry\n");
                exit(EXIT_FAILURE);
            }
            break;
        case WRITER:
            if((TelemetryProcess *)f1_telemetry == NULL)
            {
                fprintf(stderr, "Could not allocate space for the TelemetryProcess\n");
                exit(EXIT_FAILURE);
            }
            break;
        case TRACKER:
            if((F1Tracking *)f1_telemetry == NULL)
            {
                fprintf(stderr, "Could not allocate space for the F1Tracking\n");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            fprintf(stderr, "Unsupported TelemetryType\n");
            exit(EXIT_FAILURE);
    }
}

/**
 * @brief Inits the Telemetry Data Structure
 * @param Telemetry telemetry double pointer
 * @param TelemetryType type
 * @return void
 */
void init_telemetry(Telemetry **telemetry, TelemetryType type)
{
    Telemetry *t = (Telemetry *)calloc(1, sizeof(Telemetry));
    validate_telemetry(t, type);

    t->count = 0;
    t->head = 0;
    t->tail = 0;

    *telemetry = t;
}

/**
 * @brief Inits the TelemetryProcess Data Structure
 * @param TelemetryProcess telemetry_process double pointer
 * @param TelemetryType type
 * @return void
 */
void init_telemetry_process(TelemetryProcess **telemetry_process, TelemetryType type)
{
    TelemetryProcess *tp = (TelemetryProcess *)calloc(1, sizeof(TelemetryProcess));
    validate_telemetry(tp, type);
    
    tp->states = DEFAULT_STATE_MASK;

    *telemetry_process = tp;
}

/**
 * @brief Inits the F1Tracking Data Structure
 * @param F1Tracking tracker double pointer
 * @param Telemetry telemetry double pointer
 * @param TelemetryProcess process double pointer
 * @param TelemetryType type
 * @return void
 */
void init_tracking(F1Tracking **tracker, Telemetry **telemetry, TelemetryProcess **process, TelemetryType type) 
{
   F1Tracking *f1_tracker = (F1Tracking *)calloc(1, sizeof(F1Tracking));
   validate_telemetry(f1_tracker, type);

   f1_tracker->t = *telemetry;
   f1_tracker->p = *process;

   *tracker = f1_tracker;
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
 * @param F1Tracking tracker pointer
 * @return void
 */
void clean_up(F1Tracking *tracker)
{
    free(tracker->t);
    free(tracker->p);
    free(tracker);
    pthread_cond_destroy(&not_empty);
    pthread_cond_destroy(&not_full);
    pthread_mutex_destroy(&mutex);
    tracker->t = NULL;
    tracker->p = NULL;
    tracker = NULL;
}

int main(void)
{
    TelemetryType tracker = TRACKER;
    TelemetryType pthread_reader = READER;
    TelemetryType pthread_writer = WRITER;
    
    TelemetryProcess *telemetry_process = NULL;
    Telemetry *telemetry = NULL;
    F1Tracking *f1_tracker = NULL;

    init_concurrency_model();
    
    init_telemetry(&telemetry, pthread_reader);
    init_telemetry_process(&telemetry_process, pthread_writer);
    
    init_tracking(&f1_tracker, &telemetry, &telemetry_process, tracker);
    printf("F1Tracking allocated at address %p\n", f1_tracker);
    printf("Telemetry allocated at address %p\n", f1_tracker->t);
    printf("Telemetry Process allocated at address %p\n", f1_tracker->p);

    clean_up(f1_tracker);

    return 0;
}
