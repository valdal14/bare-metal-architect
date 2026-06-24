#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#define LIST_SIZE 20
#define PT_LIST_SIZE 4
#define SUM_SIZE 5

typedef struct 
{
    uint8_t *p_data;
    uint8_t start_index;
    uint8_t end_index;
} ThreadArgs;

/**
 * @brief Fills the data array 
 * @param uint8_t data pointer
 * @param size_t size 
 * @return void
 */
void fill_list(uint8_t *data, size_t size)
{
    for(uint8_t i = 0; i < size; i++)
        data[i] = (i + 1);
}

/**
 * @brief Fills the list of pthread_t
 * @param ThreadArgs list pointer
 * @param size_t size
 * @param uint8_t data pointer
 */
void fill_args_list(ThreadArgs *list, size_t size, uint8_t *data)
{
    uint8_t count = 0;
    for(uint8_t i = 0; i < size; i++)
    {
        list[i].p_data = data;
        
        for(int j = 0; j < SUM_SIZE; j++)
        {
            if(j == 0) list[i].start_index = count; 
            if(j == 4) list[i].end_index = count;
            count++;
        }
    }
}

/**
 * @brief Computes the partial sum using a thread
 * @param void arg pointer
 * @return void
 */
void *partial_sum(void *arg) {
    // Cast back to ThreadArgs
    ThreadArgs *ta = (ThreadArgs *)arg;
    
    if(ta == NULL)
    {
        fprintf(stderr, "Could not cast to ThreadArgs\n");
        exit(EXIT_FAILURE);
    }

    uint8_t total = 0;
    uint8_t *p_sum = (uint8_t *)malloc(sizeof(uint8_t)); 
    
    if(p_sum == NULL)
    {
        fprintf(stderr, "Could not allocate space for the partial sum\n");
        exit(EXIT_FAILURE);
    }
    
    for(uint8_t i = ta->start_index; i <= ta->end_index; i++)
    {
        total += ta->p_data[i];
    }

    *p_sum = total; 
    return p_sum; 
}

/**
 * @brief Spawns new threads and join them to compute the final sum 
 * @param ThreadArgs list pointer
 * @param size_t size 
 * @param uint8_t sums pointer
 * @return void
 */
void spawn_workers(ThreadArgs *list, size_t size, uint8_t *sums)
{
    pthread_t threads[4];

    // spawn the threads 
    for(uint8_t i = 0; i < size; i++)
    {
        pthread_create(&threads[i], NULL, partial_sum, (void *)&list[i]);
    }
    
    // wait for them to finish
    for(uint8_t i = 0; i < size; i++)
    {
        uint8_t *thread_result = NULL;
        pthread_join(threads[i], (void **)&thread_result);

        sums[i] = *thread_result;
        
        printf("Partial SUM: %d\n", *thread_result);

        free(thread_result);
    }

    uint8_t total = 0;

    for(uint8_t i = 0; i < size; i++)
        total += sums[i];

    printf("The total SUM is: %d\n", total);
}

int main(void)
{
    uint8_t data[LIST_SIZE] = { 0 };
    ThreadArgs args[PT_LIST_SIZE];
    uint8_t sums[PT_LIST_SIZE] = { 0 };

    fill_list(data, LIST_SIZE); 
    fill_args_list(args, PT_LIST_SIZE, data);
    
    spawn_workers(args, PT_LIST_SIZE, sums);

    return 0;
}
