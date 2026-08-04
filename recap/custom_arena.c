#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

typedef struct
{
    uint8_t *buffer;
    size_t capacity;
    size_t offset;
    size_t size;

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
    *arena = new_arena;
}

/**
 * @brief Attempts to realloc the Arena's buffer 
 * @param Arena arena pointer
 * @return void
 */
void buffer_realloc(Arena *arena)
{
    size_t new_capacity = arena->capacity * 2;
    void *op = realloc(arena->buffer, new_capacity);
    
    if(op == NULL)
    {
        fprintf(stderr, "Buffer Reallocation Failed\n");
        exit(EXIT_FAILURE);
    }

    // Point the arena buffer to the newly allocated block
    arena->buffer = (uint8_t *)op;
    arena->capacity = new_capacity; 
}

/**
 * @brief Add a new values to the Arena's Buffer
 * @param Arena arena pointer
 * @param uint8_t val
 * @param void(*on_full)(Arena *arena) realloc callback
 * @return void
 */
void add(Arena *arena, uint8_t val, void(*on_full)(Arena *arena))
{
    // get the size of the type we stored in the buffer 
    size_t type_size = sizeof(uint8_t);
    
    if((arena->size + type_size) > arena->capacity)
    {
        printf("Arena Buffer Full:\n");
        printf("Reallocating Space for the Buffer. Please Wait...\n");
        sleep(1);
        on_full(arena);
        printf("Reallocation Completed Successfully\n");
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

int main(void)
{
    Arena *arena = NULL;
    alloc(&arena, 3);
    printf("Arena allocated at address %p\n", arena);
    add(arena, 14, buffer_realloc);
    add(arena, 22, buffer_realloc);
    add(arena, 17, buffer_realloc);
    add(arena, 44, buffer_realloc);
    add(arena, 82, buffer_realloc);
    print_buffer(arena);
    return 0;
}
