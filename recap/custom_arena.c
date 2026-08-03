#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    uint8_t *buffer;
    size_t capacity;
    size_t offset;

} Arena;

/**
 * @brief Allocates the memory for the Arena allocator
 * @param Arena arena double pointer
 * @param size_t capacity
 * @return void
 */
void alloc(Arena **arena, size_t capacity)
{
    Arena *new_arena = (Arena *)calloc(1, sizeof(Arena *));
    
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

    *arena = new_arena;
}

int main(void)
{
    Arena *arena = NULL;
    alloc(&arena, 254);
    printf("Arena allocated at address %p\n", arena);
    return 0;
}
