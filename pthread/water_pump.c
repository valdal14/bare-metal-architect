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

// Concurrency Primitives
pthread_cond_t is_ready = PTHREAD_COND_INITIALIZER;
pthread_cond_t is_empty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct
{
    typedef union 
    {
        uint8_t level;
        uint8_t is_full;
    } Detail;
    /**
     * 0 is_ready    bit 
     * 2 is_empty    bit
     * 4 in_progress bit 
     */
    uint8_t state;
} Pump;


int main(void)
{
    return 0;
}
