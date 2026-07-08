#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

typedef enum
{
    ThreadPool,
    Task
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
        default:
            fprintf(stderr, "Unsupported Type\n");
            exit(EXIT_FAILURE);
    }
}

/**
 * @brief Inits the Thread Pool
 * @param threadpool_t tp double pointer
 * @param uint8_t queue_size
 * @return void
 */
void init_threadpool(threadpool_t **tp, uint8_t queue_size)
{
    threadpool_t *threadpool = (threadpool_t *)calloc(1, sizeof(threadpool_t));
    check_resource_alloc(threadpool, ThreadPool);

    *tp = threadpool;
}

int main(void)
{
    threadpool_t *tp = NULL;
    init_threadpool(&tp, 5);
    printf("threadpool_t allocated at address %p\n", tp);
    return 0;
}
