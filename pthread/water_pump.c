#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#define BIT(x) (1 << (x))
#define DEFAULT_STATE 0x00 
#define READY_BIT 0
#define EMPTY_BIT 1
#define PROGRESS_BIT 3
#define READ_LEVEL 50
#define MAX_LEVEL 100

// Concurrency Primitives
pthread_cond_t is_ready = PTHREAD_COND_INITIALIZER;
pthread_cond_t is_empty = PTHREAD_COND_INITIALIZER;
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
     */
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
   
    new_pump->temp = 77;
    new_pump->state = DEFAULT_STATE;
    *pump = new_pump;
}

int main(void)
{
    Pump *pump = NULL;
    Temp temp = F;
    init(&pump, temp);
    printf("Pump allocated at address %p with temp %.2u\n", pump, pump->temp);
    free(pump);
    return 0;
}
