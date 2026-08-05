#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

typedef struct
{
    uint8_t *buffer;
    size_t capacity;
    size_t offset;
    size_t size;
    bool is_full;

} Arena;

/**
 * @brief Allocates the memory for the Arena allocator
 * @param Arena arena double pointer
 * @param size_t capacity
 * @return void
 */
void alloc(Arena **arena, size_t capacity)
{
    Arena *new_arena = (Arena *)calloc(1, sizeof(Arena));
    
    if(new_arena == NULL)
    {
        fprintf(stderr, "Error: Could not allocate space for the Arena\n");
        exit(EXIT_FAILURE);
    }

    new_arena->buffer = (uint8_t *)calloc(capacity, sizeof(uint8_t));

    if(new_arena->buffer == NULL)
    {
        fprintf(stderr, "Error; Could not allocate space for the Buffer\n");
        exit(EXIT_FAILURE);
    }

    new_arena->capacity = capacity;
    new_arena->offset = 0;
    new_arena->size = 0;
    new_arena->is_full = false;
    *arena = new_arena;
}

/**
 * @brief Add a new values to the Arena's Buffer
 * @param Arena arena pointer
 * @param uint8_t val
 * @return void
 */
void add(Arena *arena, uint8_t val)
{
    // get the size of the type we stored in the buffer 
    size_t type_size = sizeof(uint8_t);
    
    if((arena->size + type_size) > arena->capacity)
    {
        printf("Arena Buffer Full:\n");
        printf("Start the print worker... Please wait...\n");
        sleep(1);
        pthread_cond_signal(&cond);
    }
 
    arena->buffer[arena->offset] = val;
    // the size of this is determined by the size of the type stored 
    // inside the Arena's buffer 
    arena->size += type_size;
    arena->offset += type_size;
}

/**
 * @brief Prints out the elements stored in the Arena's Buffer
 * @param Arena arena pointer
 * @return void
 */
void print_buffer(Arena *arena)
{
    for(uint8_t i = 0; i < arena->size; i++)
    {
        printf("[%d] = %d\n", i , arena->buffer[i]);
        sleep(1);
    }
}

/**
 * @brief POSIX Thread Callback used to lock 
 * and add a new value to the Arena's buffer
 * @param void arg pointer
 * @return void pointer
 */
void *worker_add(void *arg)
{
    Arena *arena = (Arena *)arg;
    pthread_mutex_lock(&lock);
    add(arena, (arena->size + 2));
    pthread_mutex_unlock(&lock);
    return (void *)arena;
}

/**
 * @brief POSIX Thread Callback used to print 
 * the values stored into the Arena's buffer
 * @param void arg pointer
 * @return void pointer
 */
void *worker_print(void *arg)
{
    Arena *arena = (Arena *)arg;
  
    pthread_mutex_lock(&lock);
    
    while(arena->size <= arena->capacity)
    {
        pthread_cond_wait(&cond, &lock);
    }
    
    print_buffer(arena);

    pthread_mutex_unlock(&lock);

    return (void *)arena;
}

int main(void)
{
    Arena *arena = NULL;
    alloc(&arena, 5);
    printf("Arena allocated at address %p\n", arena);
    size_t capacity = arena->capacity + 1;
    pthread_t workers[capacity];
    pthread_t print_thread;

    pthread_create(&print_thread, NULL, worker_print, (void *)arena);

    for(uint8_t i = 0; i < capacity; i++)
    {
        pthread_create(&workers[i], NULL, worker_add, (void *)arena);
    }

    for(uint8_t i = 0; i < capacity; i++)
    {
        pthread_join(workers[i], NULL);
    }

    pthread_join(print_thread, NULL);


    return 0;
}
