#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Triggers a heap-buffer-overflow by writing past the allocated bounds.
 * @return void
 */
void trigger_out_of_bounds()
{
    // Fix: Allocate space for 3 integers (12 bytes)
    int *data = (int *)malloc(3 * sizeof(int));
    if(data == NULL) exit(1);
    
    // Go wildly out of bounds
    for(int i = 0; i < 100000; i++)
    {
        data[i] = i; 
        // Print it so the compiler is forced to execute the code!
        printf("Writing %d to out-of-bounds memory...\n", data[i]);
    }

    free(data);
}

/**
 * @brief Triggers an abort by attempting to free the same memory block twice.
 * @return void
 */
void trigger_double_free()
{
    int *data = (int *)malloc(1 * sizeof(int));
    data[0] = 12;
    free(data);
    
    printf("First free successful. Attempting double free...\n");
    free(data); 
}

/**
 * @brief Triggers a heap-use-after-free by writing to memory that was returned to the allocator.
 * @return void
 */
void trigger_use_after_free()
{
    int *data = (int *)malloc(1 * sizeof(int));
    data[0] = 42;
    free(data);
    
    printf("Memory freed. Attempting to write to ghost memory...\n");
    data[0] = 14; 
}

int main(void)
{
    // Uncomment exactly one of these at a time to test:
    
    // trigger_out_of_bounds();
    // trigger_double_free();
    trigger_use_after_free();
    
    return 0;
}