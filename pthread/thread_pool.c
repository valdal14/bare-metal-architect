#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>
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
 * @brief Initializes the Thread Pool Engine.
 * Allocates memory for the thread pool structure, the worker threads array,
 * and the task queue ring buffer. Initializes the POSIX mutex and condition variable.
 * * @param tp Double pointer to the threadpool_t struct to be allocated and populated.
 * @param thread_count The fixed number of worker threads to spawn in the pool.
 * @param queue_size The maximum number of pending tasks the ring buffer can hold.
 * @return void (Exits the process with EXIT_FAILURE if memory allocation fails).
 */
void init_threadpool(threadpool_t **tp, uint8_t thread_count, uint8_t queue_size)
{
    threadpool_t *pool = (threadpool_t *)calloc(1, sizeof(threadpool_t));
    if(pool == NULL) exit(EXIT_FAILURE);

    pool->threads = (pthread_t *)calloc(thread_count, sizeof(pthread_t));
    if(pool->threads == NULL) exit(EXIT_FAILURE);

    pool->queue = (task_t *)calloc(queue_size, sizeof(task_t));
    if(pool->queue == NULL) exit(EXIT_FAILURE);
    
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->notify, NULL);

    pool->thread_count = thread_count;
    pool->queue_size = queue_size;
    pool->head = 0;
    pool->tail = 0;
    pool->count = 0;
    pool->shutdown = false;

    *tp = pool;
}

/**
 * @brief The infinite loop executed by every worker thread.
 * Threads sleep on a condition variable until work is available,
 * then safely extract a task from the queue and execute it.
 * * @param arg Pointer to the threadpool_t struct.
 * @return void* Returns NULL upon thread exit.
 */
void *worker_loop(void *arg)
{
    threadpool_t *pool = (threadpool_t *)arg;
    
    if(pool == NULL) return NULL;

    while(true)
    {
        pthread_mutex_lock(&pool->lock);
        
        while(pool->count == 0 && !pool->shutdown) 
        {
            pthread_cond_wait(&pool->notify, &pool->lock);
        }
        
        if(pool->shutdown && pool->count == 0) 
        {
            pthread_mutex_unlock(&pool->lock);
            break;
        }
        
        task_t task = pool->queue[pool->head];
        pool->head = (pool->head + 1) % pool->queue_size;
        pool->count--;
        
        pthread_mutex_unlock(&pool->lock);
        
        if(task.function != NULL) 
        {
            task.function(task.argument);
        }
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
    
    // Create a pool with 2 worker threads and a queue size of 10
    init_threadpool(&tp, 2, 10);
    printf("Thread pool initialized.\n");

    // Boot up the worker threads using the pthread API
    for(int i = 0; i < tp->thread_count; i++) 
    {
        pthread_create(&(tp->threads[i]), NULL, worker_loop, (void *)tp);
    }

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
    // (In a real server, this would be an infinite while(1) loop accepting HTTP requests)
    sleep(3);

    printf("Main thread: Shutting down system.\n");
    
    // Trigger graceful shutdown
    pthread_mutex_lock(&tp->lock);
    tp->shutdown = true;
    pthread_cond_broadcast(&tp->notify); // Wake everyone up to check the shutdown flag
    pthread_mutex_unlock(&tp->lock);

    // Wait for all threads to terminate safely
    for(int i = 0; i < tp->thread_count; i++) 
    {
        pthread_join(tp->threads[i], NULL);
    }

    printf("System exit successful.\n");
    return 0;
}
