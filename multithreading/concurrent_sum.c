#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#define LIST_SIZE 10000

typedef struct Data
{
    uint16_t list[LIST_SIZE];
    uint16_t chunk_size;
} Data;

typedef struct chunk_t
{
    uint16_t *list;
    uint16_t start_idx;
    uint16_t end_idx;
    uint16_t partial_sum;

} chunk_t;

typedef struct
{
    struct chunk_t *threads;
} Processing;

/**
 * @brief Creates an instance of the Data struct and 
 * populates the list's elements with value 1 at start
 * @param uint8_t num_threads
 * @return Data
 */
Data generate(uint8_t num_threads)
{
    Data data;
    for(uint16_t i = 0; i < LIST_SIZE; i++) data.list[i] = 1;
    data.chunk_size = LIST_SIZE / num_threads;
    return data;
}

/**
 * @brief Inits the Processing struct with Data and a list of pthreads
 * @param Processing processing double pointer 
 * @param uint8_t num_threads
 * @param Data(*gen_data)(uint8_t size)
 * @return void
 */
void init_processing(Processing **processing, uint8_t num_threads, Data(*gen_data)(uint8_t num_threads))
{
    Processing *proc = (Processing *)calloc(1, sizeof(Processing));

    if(proc == NULL)
    {
        fprintf(stderr, "Could not allocate space for the Processing struct\n");
        exit(EXIT_FAILURE);
    }

    // Generate the list of values 
    Data data = gen_data(num_threads);

    proc->threads = (chunk_t *)calloc(num_threads, sizeof(chunk_t));

    if(proc->threads == NULL)
    {
        fprintf(stderr, "Could not allocate space for the chunk_t threads\n");
        exit(EXIT_FAILURE);
    }

    uint16_t s_idx = 0;
    uint16_t e_idx = data.chunk_size;
    uint8_t threads_idx = 0;
    uint16_t count = 0;

    while(threads_idx < num_threads)
    {
        proc->threads[threads_idx].list = (uint16_t *)calloc(data.chunk_size, sizeof(uint16_t));
        proc->threads[threads_idx].start_idx = s_idx;
        proc->threads[threads_idx].end_idx = e_idx;
        proc->threads[threads_idx].partial_sum = 0;

        if(proc->threads[threads_idx].list == NULL)
        {
            fprintf(stderr, "Could not allocate space for the list elements\n");
            exit(EXIT_FAILURE);
        }

        for(uint16_t i = s_idx; i < e_idx; i++)
        {
            proc->threads[threads_idx].list[count] = data.list[i];
            count++;
            s_idx++;
        }

        count = 0;
        s_idx -= 1;
        e_idx += data.chunk_size;
        threads_idx++;
    }

    *processing = proc;
}

int main(void)
{
    Processing *proc = NULL;
    init_processing(&proc, 4, generate);
    
    for(uint16_t i = 0; i < 4; i++)
    {
        printf("start_idx = %d\n", proc->threads[i].start_idx);
        printf("end_idx   = %d\n", proc->threads[i].end_idx);
        for(uint16_t j = 0; j < 10; j++)
        {
            printf("[%d] = %d\n", i, proc->threads[i].list[j]);
        }
        printf("---------------------------------\n");
    }
    
    return 0;
}
