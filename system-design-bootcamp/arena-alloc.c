#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define BLOCK 256
#define BLOCK_SIZE 16 
// Concurrency variables 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

typedef struct PixelPacket
{
    char payload[8]; // 8 bytes
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

typedef struct Arena
{
    struct PixelPacket *buffer;
    size_t capacity;
    size_t offset;
} Arena;

typedef struct 
{
    struct Arena *arena;
    struct PixelPacket packet;
} TrasferPacket;

/**
 * @brief Allocates the Arena 
 * @param Arena arena double pointer
 * @return void
 */
void init(Arena **arena)
{
    Arena *new_arena = (Arena *)calloc(1, sizeof(Arena));
    assert(new_arena != NULL && "Arena allocation failed");
    new_arena->buffer = (PixelPacket *)calloc(1, BLOCK);
    assert(new_arena->buffer != NULL && "Buffer allocation failed");
    new_arena->capacity = BLOCK;
    new_arena->offset = 0;
    *arena = new_arena;
}

/**
 * @brief POSIX Thread callack used to allocate packets 
 * to the Arena 
 * @param void arg pointer
 * @return void pointer 
 */
void *add_packet(void *arg)
{
    TrasferPacket *tp = (TrasferPacket *)arg;
    pthread_mutex_lock(&lock);
    // calculate the next offset
    size_t next_offset = tp->arena->offset * BLOCK_SIZE;
    // check for out of bound memory
    if(next_offset + BLOCK_SIZE >= tp->arena->capacity)
    {
        pthread_cond_signal(&cond);
    }

    tp->arena->buffer[tp->arena->offset] = tp->packet; 
    tp->arena->offset += 1;
    pthread_mutex_unlock(&lock);
    printf("Adding PixelPacket at offset: %zu\n", tp->arena->offset);
    return (void *)tp;
}

/**
 * @brief POSIX Thread callack used to notify the completion
 * @param void arg pointer
 * @return void pointer
 */
void *report(void *arg)
{
    Arena *arena = (Arena *)arg;
    pthread_mutex_lock(&lock);
    
    while(arena->offset != BLOCK_SIZE)
    {
        pthread_cond_wait(&cond, &lock);
    }
    
    pthread_mutex_unlock(&lock);
    
    printf("Arena Max Capacity Reached\n");
    
    for(uint8_t i = 0; i < BLOCK_SIZE; i++)
    {
        printf("Value: %s\n", arena->buffer[i].payload);   
        sleep(1);
    }
    
    return NULL;
}

/**
 * @brief Destroys the Arena 
 * @param Arena arena pointer
 * @return void
 */
void destroy(Arena *arena)
{
    if(arena->buffer != NULL) free(arena->buffer);
    arena->capacity = 0;
    arena->offset = 0;
    free(arena);
}

int main(void)
{
    Arena *arena = NULL;
    init(&arena);
    printf("Arena Allocated at address %p\n", arena);
    pthread_t threads[BLOCK_SIZE];
    pthread_t rep;

    TrasferPacket packets[16];

    for(uint8_t i = 0; i < BLOCK_SIZE; i++)
    {
        PixelPacket pp;
        memcpy(pp.payload, "Payload", 8);
        pp.size = 4 * i;
        pp.flag = 0x0F;

        TrasferPacket tp;
        tp.arena = arena;
        tp.packet = pp;
        packets[i] = tp;
    }

    pthread_create(&rep, NULL, report, (void *)arena);

    for(uint8_t i = 0; i < BLOCK_SIZE; i++)
        pthread_create(&threads[i], NULL, add_packet, (void *)&packets[i]);

    for(uint8_t i = 0; i < BLOCK_SIZE; i++)
        pthread_join(threads[i], NULL);

    pthread_join(rep, NULL);

    destroy(arena);

    return 0;
}
