#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

// Concurrency variables
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_full = PTHREAD_COND_INITIALIZER;

void push(void *argument);
void pop(void *argument);

typedef struct Queue
{
    void (* push)(void *argument);
    void (* pop)(void *argument);
    uint8_t *buffer;
    bool is_full;
    uint8_t capacity;
    uint8_t size;
    uint8_t head;
    uint8_t tail;
} Queue;


typedef struct
{
    struct Queue *queue;
    uint8_t val;
} Argument;


/**
 * @brief Inits the RB Queue
 * @param Queue double pointer
 * @param uint8_t capacity
 * @return void
 */
void init(Queue **queue, uint8_t capacity)
{
    Queue *rb = (Queue *)calloc(1, sizeof(Queue));
    if(rb == NULL) exit(EXIT_FAILURE);
    
    rb->buffer = (uint8_t *)calloc(capacity, sizeof(uint8_t));
    if(rb->buffer == NULL) exit(EXIT_FAILURE);

    rb->is_full = false;
    rb->capacity = capacity;
    rb->size = 0;
    rb->head = 0;
    rb->tail = 0;
    rb->pop = pop;
    rb->push = push;
    *queue = rb;
}

/**
 * @brief Push a new value into the Queue
 * @param void argument pointer
 * @return void
 */
void push(void *argument)
{
    Argument *arg = (Argument *)argument;
    if(arg == NULL || arg->queue == NULL) exit(EXIT_FAILURE);

    if((arg->queue->size + 1) > arg->queue->capacity) 
    {
        arg->queue->is_full = true;
        pthread_cond_signal(&cond_full);
        return;
    }

    arg->queue->buffer[arg->queue->head] = arg->val;
    printf("Pushed value %d\n", arg->queue->buffer[arg->queue->head]);
    sleep(1);
    uint8_t next_index = (arg->queue->head + 1) % arg->queue->capacity;
    arg->queue->head = next_index;
    arg->queue->size += 1;
}

/**
 * @brief Pops a stored value from the Queue
 * @param void argument pointer
 * @return void
 */
void pop(void *argument)
{
    Argument *arg = (Argument *)argument;
    if(arg == NULL || arg->queue == NULL) exit(EXIT_FAILURE);
    
    if(arg->queue->size == 0) return;

    uint8_t val = arg->queue->buffer[arg->queue->tail];

    arg->queue->tail = (arg->queue->tail + 1) % arg->queue->capacity;
    arg->queue->size -= 1;
    
    printf("Popped value %d\n", val);
    arg->queue->is_full = false;
    sleep(1);
}

/**
 * @brief POSIX Thread callback that logs when
 * the queue is reached max capacity
 * @param void arg pointer
 * @return void pointer
 */
void *is_full(void *arg)
{
    Queue *queue = (Queue *)arg;
    if(queue == NULL) return NULL;
    pthread_mutex_lock(&lock);
    while(queue->is_full == false) pthread_cond_wait(&cond_full, &lock);
    printf("[WARN] The Queue Reached Max Capacity\n");
    pthread_mutex_unlock(&lock);
    return (void *)queue;
}

int main(void)
{
    Queue *queue = NULL;
    init(&queue, 3);
    printf("RB Queue allocated at address %p\n", queue);
    pthread_t t_log_full = NULL;
    pthread_detach(t_log_full);
    // spawn a thread that logs when the queue is full 
    pthread_create(&t_log_full, NULL, is_full, (void *)queue);

    // Push Values 
    Argument arg1;
    arg1.val = 14;
    arg1.queue = queue;
    // push values     
    arg1.queue->push((void *)&arg1);
    arg1.val = 7;
    arg1.queue->push((void *)&arg1);
    arg1.val = 21;
    arg1.queue->push((void *)&arg1);
    arg1.val = 28;
    arg1.queue->push((void *)&arg1);
    // pop values
    arg1.queue->pop((void *)&arg1);
    arg1.queue->pop((void *)&arg1);
    arg1.queue->pop((void *)&arg1);
    arg1.queue->pop((void *)&arg1);
    
    return 0;
}
