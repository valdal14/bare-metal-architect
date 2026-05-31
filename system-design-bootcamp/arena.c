#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 1024 * 1024 

typedef struct Arena
{
    size_t capacity;
    size_t allocated;
    void *memory;
} Arena;

/**
 * @brief Inits the Arena Allocator
 * @param Arena arena double pointer
 * @return void
 */
void init_arena(Arena **arena)
{
    Arena *mem = (Arena *)malloc(sizeof(Arena));
    if(mem == NULL) exit(1);
    mem->capacity = MAX_SIZE;
    mem->allocated = 0;
    mem->memory = (unsigned char *)calloc(1, sizeof(unsigned char) * MAX_SIZE);
    if(mem->memory == NULL) exit(1);
    *arena = mem;
}

/**
 * @brief Allocates a new block of memory based on the 
 * given size_t size and returns the next available slot
 * @param Arena arena double pointer
 * @param size_t size
 * @return void pointer
 */
void *alloc_obj(Arena *arena, size_t size)
{
    if((arena->allocated + size) > arena->capacity)
    {
        fprintf(stderr, "Max capacity reached\n");
        exit(1);
    }
    
    int reminder = size % 8;
    size_t mem_to_alloc = size;
    
    if(reminder != 0)
    {
        int padding = 8 - reminder;
        mem_to_alloc = size + padding;     
    }
  
    // get the current position from the tracker
    void *current_ptr = (unsigned char *)arena->memory + arena->allocated;
    // add the new memory block to allocate 
    arena->allocated += mem_to_alloc;
    return current_ptr;
}

/**
 * @brief Sets a new string 
 * @param Arena arena pointer
 * @param char str pointer
 * @param size_t str_size
 * @return char pointer
 */
char *set_string(Arena *arena, char *str, size_t str_size)
{
    char *text = (char *)alloc_obj(arena, str_size);
    strncpy(text, str, str_size * sizeof(char));    
    text[str_size-1] = '\0';
    return text;
}

/**
 * @brief Resets the Arena 
 * @param Arena arena pointer
 * @return void
 */
void arena_reset(Arena *arena)
{
    arena->allocated = 0;
}

/**
 * @brief Deallocate the allocated buffer
 * @param Arena arena pointer
 * @return void
 */
void destroy_arena(Arena *arena)
{
    free(arena->memory);
    free(arena);
    arena = NULL;
}


int main(void)
{
    Arena *arena = NULL;
    init_arena(&arena);

    char *str1 = set_string(arena, "Hello World!", 13);    
    printf("Set string: %s\n", str1);
    printf("Arena Allocated %zu\n", arena->allocated);

    char *str2 = set_string(arena, "Welcome to the Arena Allocator", 31);    
    printf("Set string: %s\n", str2);
    printf("Arena Allocated %zu\n", arena->allocated);
    
    arena_reset(arena);
    printf("Arena Allocated %zu\n", arena->allocated);

    // Re-Allocate new strings 
    char *new_str = set_string(arena, "Welcome Back!", 14);
    printf("Set string: %s\n", new_str);
    printf("Arena Allocated %zu\n", arena->allocated);

    destroy_arena(arena);

    return 0;
}
