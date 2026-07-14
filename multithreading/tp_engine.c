#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

/**
 * @brief The Polymorphic Task Interface.
 * Defines a generic function pointer and its argument payload.
 */
typedef struct
{
    void (*function)(void *);
    void *argument;
} task_t;

/**
 * @brief The Thread Pool Engine struct.
 * Holds the POSIX concurrency primitives, the ring buffer queue,
 * and the worker thread array.
 */
typedef struct {
    pthread_t *threads;
    task_t *queue;
    pthread_mutex_t lock;
    pthread_cond_t notify;
    uint8_t thread_count;
    uint8_t queue_size;
    uint8_t head;
    uint8_t tail;
    uint8_t count;
    bool shutdown;
} threadpool_t;

/**
 * @brief Verifies the given object allocation
 * @param obj void pointer to the allocated memory
 * @param name The name of the object being allocated for logging
 * @return void
 */
void verify_allocation(void *obj, const char *name)
{
    if(obj == NULL)
    {
        fprintf(stderr, "Could not allocate space for %s\n", name);
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("%s was successfully allocated at address %p\n", name, obj);
    }
}

/**
 * @brief Inits the ThreadPool
 * @param threadpool double pointer to the threadpool
 * @param thread_count the number of threads to spawn
 * @param queue_size the capacity of the ring buffer
 * @return void
 */
void init(threadpool_t **threadpool, uint8_t thread_count, uint8_t queue_size)
{
    threadpool_t *tp = (threadpool_t *)calloc(1, sizeof(threadpool_t));
    verify_allocation(tp, "ThreadPool");

    // init the pthread_t array dynamically
    tp->threads = (pthread_t *)calloc(thread_count, sizeof(pthread_t));
    verify_allocation(tp->threads, "Threads Array");

    // init the task_t queue dynamically
    tp->queue = (task_t *)calloc(queue_size, sizeof(task_t));
    verify_allocation(tp->queue, "Task Queue");

    // init pthread_mutex_t
    pthread_mutex_init(&tp->lock, NULL);
    // init pthread_cond_t
    pthread_cond_init(&tp->notify, NULL);

    // init the rest of the fields
    tp->thread_count = thread_count;
    tp->queue_size = queue_size;
    tp->head = 0;
    tp->tail = 0;
    tp->count = 0;
    tp->shutdown = false;

    *threadpool = tp;
}

/**
 * @brief POSIX Threads callback used to manage the lifecycle of the
 * spawned pthreads.
 * @param void arg pointer
 * @return void pointer
 */
void *worker_loop(void *arg)
{
    threadpool_t *pool = (threadpool_t *)arg;
    verify_allocation(pool, "ThreadPool");

    while(true)
    {
        pthread_mutex_lock(&pool->lock);
        // while the queue is empty and shutdown is false wait
        while(pool->count == 0 && !pool->shutdown) pthread_cond_wait(&pool->notify, &pool->lock);
        // if shutdown is true and the queue is empty release the lock and break 
        if(pool->shutdown && pool->count == 0)
        {
            pthread_mutex_unlock(&pool->lock);
            break;
        }

        task_t task = pool->queue[pool->head];
        pool->head = (pool->head + 1) % pool->queue_size;
        pool->count--;

        // execute the function 
        if(task.function != NULL) task.function(task.argument);
    }

    return NULL;
}

/**
 * @brief Submits a new task to the thread pool queue.
 * Locks the queue, inserts the polymorphic function and argument,
 * advances the tail pointer, and signals a sleeping worker thread.
 * * @param pool Pointer to the threadpool engine.
 * @param function The polymorphic function to execute (must return void).
 * @param argument The payload/data to pass to the function.
 * @return bool True if accepted, False if the queue is full (HTTP 429).
 */
bool submit_task(threadpool_t *pool, void (*function)(void *), void *argument)
{
    pthread_mutex_lock(&pool->lock);
    
    if(pool->count == pool->queue_size)
    {
        pthread_mutex_unlock(&pool->lock);
        return false;
    }

    pool->queue[pool->tail].function = function;
    pool->queue[pool->tail].argument = argument;

    pool->tail = (pool->tail + 1) % pool->queue_size;
    pool->count++;

    pthread_cond_signal(&pool->notify);
    pthread_mutex_unlock(&pool->lock);

    return true;
}

/**
 * @brief Example task function to be executed by the thread pool.
 * Unpacks the void pointer into a uint8_t array and simulates work.
 * * @param arg Void pointer to the arguments array.
 * @return void
 */
void execute_task(void *arg)
{
    uint8_t *args = (uint8_t *)arg;

    // Get the pthread ID for logging purposes
    pthread_t id = pthread_self();

    printf("[Thread %p] Executing task with args: {%d, %d}...\n", (void*)id, args[0], args[1]);

    // Simulate a blocking operation (like a DB query or API call)
    sleep(1);

    printf("[Thread %p] Task complete.\n", (void*)id);
}


int main(void)
{
    threadpool_t *tp = NULL;
    init(&tp, 16, 5);
    printf("ThreadPool allocated at address %p\n", tp);
    return 0;
}
