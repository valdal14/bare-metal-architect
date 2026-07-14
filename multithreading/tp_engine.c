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

int main(void)
{
    threadpool_t *tp = NULL;
    init(&tp, 16, 5);
    printf("ThreadPool allocated at address %p\n", tp);
    return 0;
}
