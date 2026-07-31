#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BLOCK 256

typedef struct PixelPacket
{
    char *payload; // 8 bytes
    uint32_t size; // 4 bytes
    /**
     * bit 0 valid  
     * bit 2 red
     * bit 4 green
     * bit 8 blue
     */
    uint8_t flag; // 1 bytes 
    uint8_t padding[3]; // 3 bytes 
    // 16 bytes struct size 
} PixelPacket;

typedef struct
{
    struct PixelPacket *buffer;
    size_t capacity;
    size_t offset;
} Arena;


/**
 * @brief Allocates the Arena 
 * @param Arena arena double pointer
 * @return void
 */
void init(Arena **arena)
{
    Arena *new_arena = (Arena *)calloc(1, sizeof(Arena));
    
    if(new_arena == NULL)
    {
        fprintf(stderr, "Could not allocate space for the Arena\n");
        exit(EXIT_FAILURE);
    }

    new_arena->buffer = (PixelPacket *)calloc(1, BLOCK);

    if(new_arena->buffer == NULL)
    {
        fprintf(stderr, "Could not allocate space for the buffer\n");
        exit(EXIT_FAILURE);
    }

    new_arena->capacity = BLOCK;
    new_arena->offset = 0;

    *arena = new_arena;
}

int main(void)
{
    Arena *arena = NULL;
    init(&arena);
    printf("Arena Allocated at address %p\n", arena);
    return 0;
}
