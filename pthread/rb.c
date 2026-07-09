#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_QUEUE_SIZE 5
#define MAX_THREAD_SIZE 10

typedef enum
{
    ThreadPool,
    Task,
    Thread
} T_Type;

typedef struct
{
    void(*function)(void*);
    void *arguments;
} task_t;

typedef struct 
{
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
 * @brief Verifies the allocation of a given object represented
 * by the void pointer and idenfiable by the T_Type enum type
 * @param void obj pointer
 * @param T_Type type enum 
 * @return void
 */
void check_resource_alloc(void *obj, T_Type type)
{
    switch(type)
    {
        case ThreadPool:
            if((threadpool_t *)obj == NULL)
            {
                fprintf(stderr, "Could not cast the given object to a threadpool_t resource\n");
                exit(EXIT_FAILURE);
            }
            break;
        case Task:
            if((task_t *)obj == NULL)
            {
                fprintf(stderr, "Could not cast the given object to a task_t resource\n");
                exit(EXIT_FAILURE);
            }
            break;
        case Thread:
            if((pthread_t *)obj == NULL)
            {
                fprintf(stderr, "Could not cast the given object to a pthread_t resource\n");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            fprintf(stderr, "Unsupported Type\n");
            exit(EXIT_FAILURE);
    }
}

/**
 * @brief Inits the Thread Pool
 * @param threadpool_t tp double pointer
 * @return void
 */
void init_threadpool(threadpool_t **tp)
{
    threadpool_t *threadpool = (threadpool_t *)calloc(1, sizeof(threadpool_t));
    check_resource_alloc(threadpool, ThreadPool);

    threadpool->queue = (task_t *)calloc(MAX_QUEUE_SIZE, sizeof(task_t));
    check_resource_alloc(threadpool->queue, Task);

    threadpool->threads = (pthread_t *)calloc(MAX_THREAD_SIZE, sizeof(pthread_t));
    check_resource_alloc(threadpool->threads, Thread);

    threadpool->queue_size = MAX_QUEUE_SIZE;
    threadpool->thread_count = MAX_THREAD_SIZE;
    threadpool->head = 0;
    threadpool->tail = 0;
    threadpool->count = 0;
    threadpool->shutdown = false;

    *tp = threadpool;
}

/**
 * @brief POSIX thread callbacks used to executes a task 
 * @param void arg pointer
 * @return void pointer
 */
void *execute_task(void *arg)
{
    threadpool_t *tp = (threadpool_t *)arg;
    check_resource_alloc(tp, ThreadPool);
    
    while(true)
    {
        pthread_mutex_lock(&tp->lock);

        while(tp->count == 0 && !tp->shutdown)
        {
            pthread_cond_wait(&tp->notify, &tp->lock);
        }

        if(tp->shutdown && tp->count == 0)
        {
            pthread_mutex_unlock(&tp->lock);
            break;
        }

        task_t task = tp->queue[tp->head];
        tp->head = (tp->head + 1) % MAX_QUEUE_SIZE;

        tp->count -= 1;

        pthread_mutex_unlock(&tp->lock);

        if(task.function != NULL) task.function(task.arguments);
    }

    return NULL;
}

int main(void)
{
    threadpool_t *tp = NULL;
    init_threadpool(&tp);
    printf("threadpool_t allocated at address %p\n", tp);
    return 0;
}
