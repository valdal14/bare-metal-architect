#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

// The Polymorphic Task Interface
typedef struct
{
    void (*function)(void *);
    void *argument;
} task_t;

// The Thread Pool Engine
typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t notify;
    pthread_t *threads;
    task_t *queue;
    uint8_t thread_count;
    uint8_t queue_size;
    uint8_t head;
    uint8_t tail;
    uint8_t count;
    bool shutdown;
} threadpool_t;

/**
 * @brief Initializes the Thread Pool Engine
 * * Allocates memory for the thread pool structure, the worker threads array,
 * and the task queue ring buffer. Initializes the POSIX mutex and condition variable.
 * @param tp Double pointer to the threadpool_t struct to be allocated and populated.
 * @param thread_count The fixed number of worker threads to spawn in the pool.
 * @param queue_size The maximum number of pending tasks the ring buffer can hold.
 * @return void (Exits the process with EXIT_FAILURE if memory allocation fails).
 */
void init_threadpool(threadpool_t **tp, uint8_t thread_count, uint8_t queue_size)
{
    // Allocate space for the Thread Pool Engine
    threadpool_t *threadpool = (threadpool_t *)calloc(1, sizeof(threadpool_t));
    if(threadpool == NULL)
    {
        fprintf(stderr, "ERROR: Could not allocate space for the threadpool\n");
        exit(EXIT_FAILURE);
    }

    // Allocate space for the threads array using the dedicated thread_count
    threadpool->threads = (pthread_t *)calloc(thread_count, sizeof(pthread_t));
    if(threadpool->threads == NULL)
    {
        fprintf(stderr, "ERROR: Could not allocate space for the threads array\n");
        exit(EXIT_FAILURE);
    }

    // Allocate space for the queue ring buffer using the dedicated queue_size
    threadpool->queue = (task_t *)calloc(queue_size, sizeof(task_t));
    if(threadpool->queue == NULL)
    {
        fprintf(stderr, "ERROR: Could not allocate space for the queue\n");
        exit(EXIT_FAILURE);
    }
    
    // Init the mutex and condition variable
    pthread_mutex_init(&threadpool->lock, NULL);
    pthread_cond_init(&threadpool->notify, NULL);

    // Initialize state
    threadpool->thread_count = thread_count;
    threadpool->queue_size = queue_size;
    threadpool->head = 0;
    threadpool->tail = 0;
    threadpool->count = 0;
    threadpool->shutdown = false;

    // Mutate the caller's pointer
    *tp = threadpool;
}

/**
 * @brief POSIX Threads callback 
 * @param void arg pointer
 * @return void pointer
 */
void *worker_loop(void *arg)
{
    threadpool_t *pool = (threadpool_t *)arg;
    
    if(pool == NULL)
    {
        fprintf(stderr, "ERROR: Could not cast the value to a task_t\n");
        exit(EXIT_FAILURE);
    }

    while(true)
    {
        pthread_mutex_lock(&pool->lock);
        
        while(pool->count == 0 && !pool->shutdown)
        {
            pthread_cond_wait(&pool->notify, &pool->lock);
        }

        // Shutdown check
        if(pool->shutdown && pool->count == 0)
        {
            pthread_mutex_unlock(&pool->lock);
            // Exit the while loop and kill the thread
            break; 
        }

        // Grab the task
        task_t task = pool->queue[pool->head];
        // Update the Ring Buffer
        pool->head = (pool->head + 1) % pool->queue_size;

        // Decrement the pending task count
        pool->count--;

        // Unlock the lock 
        pthread_mutex_unlock(&pool->lock);

        // Execute the function
        if(task.function != NULL) task.function(task.argument);

    }

    return NULL;
}

int main(void)
{
    threadpool_t *threadpool = NULL;
    // Request 4 worker threads, but a queue capacity of 100
    init_threadpool(&threadpool, 4, 100);
    printf("threadpool_t allocated at address %p\n", (void*)threadpool);
    return 0;
}
