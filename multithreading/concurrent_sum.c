#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#define LIST_SIZE 10000

typedef struct Data
{
    uint16_t *list;
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
    uint16_t chunk_size;
    uint16_t *master_list;
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
    data.list = (uint16_t *)calloc(LIST_SIZE, sizeof(uint16_t));

    if (data.list == NULL) 
    {
        fprintf(stderr, "Master list allocation failed\n");
        exit(EXIT_FAILURE);
    }

    for(uint16_t i = 0; i < LIST_SIZE; i++)
        data.list[i] = 1;

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
    proc->chunk_size = data.chunk_size;
    proc->master_list = data.list;

    proc->threads = (chunk_t *)calloc(num_threads, sizeof(chunk_t));

    if(proc->threads == NULL)
    {
        fprintf(stderr, "Could not allocate space for the chunk_t threads\n");
        exit(EXIT_FAILURE);
    }

    uint16_t s_idx = 0;
    uint16_t e_idx = data.chunk_size;
    uint8_t threads_idx = 0;

    while(threads_idx < num_threads)
    {
        proc->threads[threads_idx].list = data.list;
        proc->threads[threads_idx].start_idx = s_idx;
        proc->threads[threads_idx].end_idx = e_idx;
        proc->threads[threads_idx].partial_sum = 0;

        s_idx += data.chunk_size;
        e_idx += data.chunk_size;
        threads_idx++;
    }
    
    *processing = proc;
}

/**
 * @brief Computes the partial sum for every POSIX 
 * thread callback executed
 * @param void arg pointer 
 * @return void pointer
 */
void *compute_partial_sum(void *arg)
{
    chunk_t *chunk = (chunk_t *)arg;

    if(chunk == NULL || chunk->list == NULL)
    {
        fprintf(stderr, "Could not cast to type chunk_t or the list is empty\n");
        exit(EXIT_FAILURE);
    }

    uint16_t count = 0;

    for(uint16_t i = chunk->start_idx; i < chunk->end_idx; i++)
    {
        chunk->partial_sum += chunk->list[count];
        count++;
    }

    count = 0;
    return NULL;
}

/**
 * @brief Spawns the threads and perform the computation
 * @param Processing proc pointer
 * @param uint8_t num_threads
 * @return void
 */
void parallel_compute(Processing *proc, uint8_t num_threads)
{
    pthread_t threads[num_threads];
    uint16_t total = 0;

    for(uint8_t i = 0; i < num_threads; i++)
    {
        pthread_create(&threads[i], NULL, compute_partial_sum, (void *)&proc->threads[i]);
    }

    for(uint8_t i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    for(uint8_t i = 0; i < num_threads; i++)
    {
        total += proc->threads[i].partial_sum;
        printf("Thread[%d] partial sum = %d\n", i, proc->threads[i].partial_sum);
        printf("Total = %d\n", total);
        sleep(1);
    }
}

/**
 * @brief Cleanups the dynamically allocated memory for the processing engine.
 * Frees the master data list, the thread structures, and the engine itself.
 * @param proc Pointer to the Processing struct to be destroyed.
 * @return void
 */
void cleanup(Processing *proc)
{
    if (proc == NULL) return;

    if (proc->master_list != NULL)
    {
        free(proc->master_list);
    }

    if (proc->threads != NULL)
    {
        free(proc->threads);
    }

    free(proc);

    printf("\nMemory Cleanup Complete.\n");
}

int main(void)
{
    Processing *proc = NULL;
    init_processing(&proc, 4, generate);
    parallel_compute(proc, 4);
    cleanup(proc);

    return 0;
}
