#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#define BIT(x) (1 << (x))
#define DEFAULT_STATE 0x02 
#define READY_STATE 0x09
#define READY_BIT 0
#define EMPTY_BIT 1
#define PROGRESS_BIT 3
#define MAX_LEVEL 100

// Concurrency Primitives
pthread_cond_t is_ready = PTHREAD_COND_INITIALIZER;
pthread_cond_t is_level = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef enum
{
    C,
    F
} Temp;

typedef struct
{
    Temp temp;
    union
    {
        double cel;
        double far;
    };
    /**
     * 0 is_ready    bit 
     * 2 is_empty    bit
     * 4 in_progress bit 
     * 1001
     */
    uint8_t level;
    uint8_t state;
} Pump;

/**
 * @brief Helpers to show the Temp value based on the Temp Type
 * @param Pump pump pointer
 * @param Temp temp
 * @return void
 */
void get_temp(Pump *pump, Temp temp)
{
    switch(temp)
    {
        case C:
        {    
            double c_temp = (pump->far * 1.8) + 32;
            printf("Current Temp in C = %.2f\n", c_temp);
            break;
        }
        case F:
        {   
            double f_temp = (pump->cel - 32) / 1.8;
            printf("Current Temp in F = %.2f\n", f_temp);
            break;
        }
        default:
            fprintf(stderr, "Unsupported Temp Type Error\n");
            exit(EXIT_FAILURE);
    }
}

/**
 * @brief Inits the Pump to its default state
 * @param Pump pump double pointer
 * @param Temp temp
 * @return void
 */
void init(Pump **pump, Temp temp)
{
    Pump *new_pump = (Pump *)calloc(1, sizeof(Pump));
    if(pump == NULL) exit(EXIT_FAILURE);
   
    switch(temp)
    {
        case C:
            new_pump->temp = C;
            break;
        case F:
            new_pump->temp = F;
            break;
        default:
            fprintf(stderr, "Unsupported Temp Type. Switching to Celcius by default\n");
            new_pump->temp = C;
            break;
    }
   
    new_pump->level = 0;
    new_pump->temp = 77;
    new_pump->state = DEFAULT_STATE;
    *pump = new_pump;
}

void *fill_water(void *arg)
{
    Pump *pump = (Pump *)arg;
    if(pump == NULL) exit(EXIT_FAILURE);
    pthread_mutex_lock(&mutex);
    
    while((pump->state & BIT(1)) != 0)
    {
        pump->level += 10;
        // set the pump state ready 
        if(pump->level >= MAX_LEVEL) 
        {
            pump->state &= ~(BIT(1));
            pthread_cond_signal(&is_ready);
        }
    }
    
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&is_ready);
    return (void *)pump;
}

void *get_water(void *arg)
{
    Pump *pump = (Pump *)arg;
    if(pump == NULL) exit(EXIT_FAILURE);
    printf("Waiting for the water level to reach min capacity\n");
    pthread_mutex_lock(&mutex);
    while((pump->state & READY_STATE) != 0) pthread_cond_wait(&is_ready, &mutex);
    pthread_mutex_unlock(&mutex);
     
    // get water 
    while((pump->state & BIT(1)) == 0)
    {
        printf("Getting 10 liters of water our of the tank [%d]\n", pump->level);
        sleep(1);
        pthread_mutex_lock(&mutex);
        pump->level -= 10;
        if(pump->level == 0)
        {
            // stop the pump form getting water 
            pump->state |= BIT(1);
        }
        pthread_mutex_unlock(&mutex);
    }

    return (void *)pump;
}

void *show_report(void *arg)
{
    Pump *pump = (Pump *)arg;
    if(pump == NULL) exit(EXIT_FAILURE);
    pthread_mutex_lock(&mutex);
    while((pump->state & BIT(1)) != 0) pthread_cond_wait(&is_level, &mutex);
    get_temp(pump, F);
    pthread_mutex_unlock(&mutex);
    return (void *)pump;
}

int main(void)
{
    Pump *pump = NULL;
    Temp temp = F;
    pthread_t device;
    pthread_t user;
    pthread_t report;
    init(&pump, temp);
    printf("Pump allocated at address %p with temp %.2u\n", pump, pump->temp);
    // Spawn the workers 
    pthread_create(&device, NULL, fill_water, (void *)pump);
    pthread_create(&user, NULL, get_water, (void *)pump);
    pthread_create(&report, NULL, show_report, (void *)pump);
    // join the threads 
    pthread_join(device, (void **)pump);
    pthread_join(user, (void **)pump);
    pthread_join(report, (void **)pump);



    free(pump);
    return 0;
}
