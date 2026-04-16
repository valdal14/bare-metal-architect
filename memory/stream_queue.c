#include <stdio.h>
#include <stdlib.h>

struct DataChunk
{
    int id;
    int payload_size;
};

struct DataChunk** init_queue(int capacity)
{
    struct DataChunk **queue = (struct DataChunk**)calloc(capacity, sizeof(struct DataChunk *));
    
    if(queue == NULL)
    {
        fprintf(stderr, "Error allocating memory space for the queue");
        exit(1);
    }

    return queue;
}

void enqueue(struct DataChunk **queue, int capacity, int *current_size, int id, int size)
{
   if(*current_size == capacity)
    {
        fprintf(stderr, "Queue is full\n");
        exit(1);
    }

    queue[*current_size] = (struct DataChunk*)malloc(sizeof(struct DataChunk));
    
    if(queue[*current_size] == NULL)
    {
        fprintf(stderr, "Cannot allocate space for single DataChunk \n");
        return;
    }

    queue[*current_size]->id = id;
    queue[*current_size]->payload_size = size;
    *current_size += 1;
}


void dequeue(struct DataChunk **queue, int *current_size)
{
    if(*current_size == 0)
    {
        fprintf(stderr, "Queue is empty\n");
        exit(1);
    }
    
    // chunk to pop from the queue
    struct DataChunk* chunk = queue[0];
    
    if(chunk == NULL)
    {
        printf("EXIT");
        exit(1);
    }

    printf("chunk id = %d will be  dequeued...\n", chunk->id);

    for(int i = 1; i < *current_size; i++)
    {
        queue[i - 1] = queue[i];
        
        if(*current_size - 1 == i)
        {
            queue[i] = NULL;
        }
    }
    
    *current_size -= 1;
    free(chunk);
}


void teardown_queue(struct DataChunk **queue, int capacity)
{
    for(int i = 0; i < capacity; i++)
        free(queue[i]);

    free(queue);
}

void show(struct DataChunk **queue, int *size)
{
    printf("\n --- Current Queue --- \n");

    for(int i = 0; i < *size; i++)
    {
        struct DataChunk* chunk = queue[i];
        printf("Chunk id: %d - payload size: %d\n", chunk->id, chunk->payload_size);
    }
}

int main(void)
{

    const int capacity = 5;
    int current_capacity = 0;

    struct DataChunk **queue = init_queue(capacity);
    enqueue(queue, capacity, &current_capacity, 111, 144);
    printf("current capacity: %d\n", current_capacity);
    
    enqueue(queue, capacity, &current_capacity, 112, 74);
    printf("current capacity: %d\n", current_capacity);
    
    enqueue(queue, capacity, &current_capacity, 113, 68);
    printf("current capacity: %d\n", current_capacity);

    enqueue(queue, capacity, &current_capacity, 114, 868);
    printf("current capacity: %d\n", current_capacity);
    
    // Will not be added to the queue
    enqueue(queue, capacity, &current_capacity, 115, 11);
    printf("current capacity: %d\n", current_capacity);
    
    dequeue(queue, &current_capacity);
    printf("current capacity: %d\n", current_capacity);
    
    dequeue(queue, &current_capacity);
    printf("current capacity: %d\n", current_capacity);
    
    show(queue, &current_capacity);

    teardown_queue(queue, capacity);
    return 0;
}
