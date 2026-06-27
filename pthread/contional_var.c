#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#define BIT(x) (1 << (x))
#define DEFAULT_MASK 0x00
#define READY_MASK 0x07

// declare and init a pthread conditional variable
pthread_cond_t is_full = PTHREAD_COND_INITIALIZER;
// declare and init a pthread mutex variable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct 
{
    /**
     * clean: bit 0
     * full : bit 2
     * open : bit 4
     * 0000
     * 1011 = 7 base 10 
     */
    uint8_t state;
} Pool;

/**
 * @brief Inits the Pool with default values
 * @param Pool pool double pointer 
 * @return void
 */
void init_pool(Pool **pool)
{
    Pool *p = (Pool *)calloc(1, sizeof(Pool));
    if(p == NULL) exit(EXIT_FAILURE);
    
    p->state = DEFAULT_MASK;
    *pool = p; 
}

int main(void)
{
    Pool *pool = NULL;
    init_pool(&pool);
    printf("Pool space allocated at %p\n", pool);
    return 0;
}
