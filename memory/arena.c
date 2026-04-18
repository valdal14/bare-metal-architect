#include <stdio.h>
#include <stdlib.h>

#define ARENA 0
#define BLOCK 1

struct Arena
{
    // raw buffer
    char *memory_block;
    // size of the buffer
    size_t total_size;
    // size offset
    size_t offset;
};

/**
 * @brief Check the memory allocation based on the given type
 * @param *memory A void pointer
 * @param type The integer value associated with the Arena struct or its
 * internal char pointer *memory_block
 * @return 1 if allocation is successful 0 if not
 */
int check_memory_alloc(void *memory, int type)
{
    switch (type)
    {
        case 0:
            if(memory == NULL)
            {
                fprintf(stderr, "ERNO00: Arena could not be allocated\n");
                return 0;
            }
            return 1;
        case 1:
            if((char *)memory == NULL)
            {
                fprintf(stderr, "ERNO01: Memory Block could not be allocated\n");
                return 0;
            }
            return 1;
        default:
            fprintf(stderr, "Unsupported type error\n");
            return 0;
    }

    return 0;
}

/**
 * @brief Init the Arena and the memory block
 * @param size_t The size of the block we want to allocate
 * @return Arena struct pointer
 */
struct Arena *init_arena(size_t size)
{
    char *mem_block = (char *)malloc(size * sizeof(char));
    struct Arena *arena = (struct Arena *)malloc(sizeof(struct Arena));
    
    if(check_memory_alloc(mem_block, BLOCK) == 0 || check_memory_alloc(arena, ARENA) == 0)
    {
        exit(1);
    }

    arena->memory_block = mem_block;
    arena->total_size = size;
    arena->offset = 0;

    return arena;

}

/**
 * @brief Check the status of the Arena allocation
 * @param struct Arena pointer
 * @param size_t The bytes we want to allocate
 * @return void pointer
 */
void *arena_alloc(struct Arena *a, size_t bytes_requested)
{
    if((a->offset + bytes_requested) > a->total_size)
    {
        fprintf(stderr, "ERNO02 - Arena OOM\n");
        exit(1);
    }

    // calculate the current free address
    char *current_ptr = a->memory_block + a->offset;
    // move the offset forward
    a->offset += bytes_requested;

    return current_ptr;
}


/**
 * @brief Reset the offset of the Arena
 * @param struct Arena pointer
 * @return void
 */
void arena_reset(struct Arena *a)
{
    a->offset = 0;
}

/**
 * @brief Destroy the Arena memory block
 * @param struct Arena pointer
 * @return void
 */
void arena_destroy(struct Arena *a)
{
    free(a->memory_block);
    free(a);
}

/**
 * @brief Output the current state of the Arena allocation
 * @param void pointer associated to the single arena allocation 
 * @param size The size of the requested block 
 * @param int The value of the current total allocation for the block
 */
void show_alloc_status(void *allocation, int size, int current_offset)
{
    printf("Address = %p\n", allocation);
    // write the allocated space to the allocation pointer
    *(int *)allocation = size;
    printf("Allocated space = %d\n", *(int *)allocation);
    printf("Current Arena Offset = %d\n", current_offset);
    printf("-------------------------------------\n");
}

int main(void)
{
    struct Arena *arena = init_arena(1024);
    
    // Testing the Arena Allocation ---
    void *a_ptr1 = arena_alloc(arena, 512);
    show_alloc_status(a_ptr1, 512, arena->offset);

    void *a_ptr2 = arena_alloc(arena, 128);
    show_alloc_status(a_ptr2, 128, arena->offset);

    void *a_ptr3 = arena_alloc(arena, 128);
    show_alloc_status(a_ptr3, 128, arena->offset);
   
    void *a_ptr4 = arena_alloc(arena, 512);
    show_alloc_status(a_ptr4, 512, arena->offset);

    arena_reset(arena);
    arena_destroy(arena);
    return 0;
}
