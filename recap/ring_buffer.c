#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

void push(void *argument);
uint8_t pop(void *argument);

typedef struct 
{
    void (* push)(void *argument);
    uint8_t (* pop)(void *argument);
    uint8_t *buffer;
    bool is_full;
    uint8_t capacity;
    uint8_t size;
    uint8_t head;
    uint8_t tail;
} Queue;

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

    *queue = rb;
}

int main(void)
{
    Queue *queue = NULL;
    init(&queue, 10);
    printf("RB Queue allocated at address %p\n", queue);

    return 0;
}
