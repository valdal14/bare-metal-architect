#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

typedef struct 
{
    uint32_t counter_A;
    // 16 integers * 4 bytes = 64 bytes of dead space!
    // This pushes counter_B completely out of the L1 Cache Line.
    uint32_t padding[16]; 
    uint32_t counter_B;
} Counters;

/**
 * @brief Inits the Counters struct 
 * @param Counters counters double pointer
 * @return void
 */
void init_counters(Counters **counters)
{
    Counters *new = (Counters *)calloc(1, sizeof(Counters));
    if(new == NULL) exit(EXIT_FAILURE);

    new->counter_A = 0;
    new->counter_B = 0;

    *counters = new;
}

/**
 * @brief Thread Callback
 * @param void arg pointer
 * @return void pointer
 */
void *thread_a_routine(void *arg)
{
    Counters *counters = (Counters *)arg;

    for(uint32_t i = 0; i < 500000000; i++)
        counters->counter_A++;

    return (void *)&counters->counter_A;
}

/**
 * @brief Thread Callback
 * @param void arg pointer
 * @return void pointer
 */
void *thread_b_routine(void *arg)
{
    Counters *counters = (Counters *)arg;
    
    for(uint32_t i = 0; i < 500000000; i++)
        counters->counter_B++;

    return (void *)&counters->counter_B;
}

int main(void)
{
    Counters *counters = NULL;
    init_counters(&counters);
    printf("Counters allocated at address %p\n", counters);
    pthread_t t[2];
    uint32_t *a_counter, *b_counter = NULL;

    pthread_create(&t[0], NULL, thread_a_routine, (void *)counters);
    pthread_create(&t[1], NULL, thread_b_routine, (void *)counters);

    pthread_join(t[0], (void **)&a_counter);
    pthread_join(t[1], (void **)&b_counter);

    printf("A Counter = %d\n", *a_counter);
    printf("B Counter = %d\n", *b_counter);

    free(counters);
    return 0;
}
