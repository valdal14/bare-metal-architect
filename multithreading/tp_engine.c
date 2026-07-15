#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

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
 * @brief Helper used to calculate the runtime 
 * of the worker_loop function to get the
 * This helper helps to compute L=λW to calculate
 * the size of the queue.
 *
 * @param task_t task
 * @return void
 */
void _calculate_runtime(task_t task)
{
    struct timespec start, end;
    
    clock_gettime(CLOCK_MONOTONIC, &start); 
    task.function(task.argument); 
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    // Calculate total seconds (Wait)
    double W = (end.tv_sec - start.tv_sec) + 
               (end.tv_nsec - start.tv_nsec) / 1e9;
               
    printf("Task Execution Time (W): %f seconds\n", W);
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
        pthread_mutex_unlock(&pool->lock);

        // calculate the runtime and execute the task 
        if(task.function != NULL) 
        {
            _calculate_runtime(task);
        }
    }

    return NULL;
}

/**
 * @brief Helper used to boot up the workers.
 * The POSIX thread will be sleeping until 
 * a new request will be submitted. 
 *
 * @param threadpool_t tp pointer
 * @return void
 */
void spawn_workers(threadpool_t *tp)
{
    for(int i = 0; i < tp->thread_count; i++)
        pthread_create(&(tp->threads[i]), NULL, worker_loop, (void *)tp);
}

/**
 * @brief Helper used to join the workers.
 * with the Main thread.
 * @param threadpool_t tp pointer
 * @return void
 */
void join_workers(threadpool_t *tp)
{
    for(int i = 0; i < tp->thread_count; i++)
        pthread_join(tp->threads[i], NULL);
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
    init(&tp, 16, 4);
    // Boot up the workers
    spawn_workers(tp);
    
    // Prepare some payloads
    uint8_t payload1[2] = {10, 4};
    uint8_t payload2[2] = {99, 1};
    uint8_t payload3[2] = {42, 7};
    uint8_t payload4[2] = {15, 3};

    // The Main Thread (like an API Gateway) floods the queue
    printf("Main thread: Submitting 4 tasks...\n");
    submit_task(tp, execute_task, payload1);
    submit_task(tp, execute_task, payload2);
    submit_task(tp, execute_task, payload3);
    submit_task(tp, execute_task, payload4);

    printf("Main thread: Tasks submitted. Now waiting for workers to finish...\n");

    // Sleep main thread to give workers time to process the queue
    // (In a real server, this would be an infinite while(true) loop accepting HTTP requests)
    sleep(3);

    printf("Main thread: Shutting down system.\n");

    // Trigger graceful shutdown
    pthread_mutex_lock(&tp->lock);
    tp->shutdown = true;
    // Wake everyone up to check the shutdown flag
    pthread_cond_broadcast(&tp->notify); 
    pthread_mutex_unlock(&tp->lock);

    // Wait for all threads to terminate safely
    join_workers(tp);
    
    printf("System exit successful.\n");

    return 0;
}
