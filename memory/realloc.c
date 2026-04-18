#include <stdio.h>
#include <stdlib.h>

struct DynamicArray
{
    int *data;
    size_t capacity;
    size_t count;
};

/**
 * @brief Initializes a DynamicArray with a starting capacity.
 * @param capacity The initial number of integer slots to allocate.
 * @return A pointer to the newly allocated DynamicArray struct.
 */
struct DynamicArray *init_array(size_t capacity)
{
    struct DynamicArray *array = (struct DynamicArray *)malloc(sizeof(struct DynamicArray));
    
    if(array == NULL)
    {
        fprintf(stderr, "Error allocating space for the array\n");
        exit(1);
    }

    array->data = (int *)calloc(capacity, sizeof(int));
    array->capacity = capacity;
    array->count = 0;
    
    return array;
}

/**
 * @brief Appends an integer to the dynamic array, resizing if capacity is reached.
 * @param array Pointer to the DynamicArray struct.
 * @param value The integer to append.
 * @return void
 */
void append_item(struct DynamicArray *array, int value)
{
    if(array->count == array->capacity)
    {
        printf("Resizing in progress...\n"); 
        printf("Current capacity: %zu\n", array->capacity);
        
        // Double the capacity logically
        array->capacity = array->capacity * 2;
        
        // Safely reallocate using BYTES
        void *new_buffer = realloc(array->data, array->capacity * sizeof(int));
        
        if(new_buffer == NULL)
        {
            fprintf(stderr, "Fatal: Cannot resize the current array. OOM.\n");
            exit(1); 
        }
        
        // Update the pointer
        array->data = (int *)new_buffer;
        printf("Resized array to capacity %zu. New address: %p\n", array->capacity, (void*)array->data);
    }
    
    // Append the new value
    printf("Adding new value %d to the array...\n", value);
    array->data[array->count] = value;
    array->count += 1;
}

/**
 * @brief Output the statistic of the DynamicArray
 * @param DynamicArray struct pointer
 * @return void
 */
void print_values(struct DynamicArray *array)
{
    printf("\n--- DynamicArray Information ---\n");
    printf("Values count stored = %zu\n", array->count);
    printf("Current Dynamic Arrays capacity = %zu\n", array->capacity);
    
    // get the current size of the array
    int current_size = array->count;
    
    printf("---------------------------------\n"); 
    printf("List of stored values: \n");

    for(int i = 0; i < current_size; i++)
        printf("At index %d is stored the value: %d\n", i, array->data[i]);
}

/**
 * @brief Free the data stored in the DA and the DA itself
 * @param DynamicArray struct pointer
 * @return void 
 */
void free_array(struct DynamicArray *array)
{
    free(array->data);
    free(array);
}

int main(void)
{
    struct DynamicArray *array = init_array(10);
    int values[11] = { 14, 7, 23, 112, 11, 45, 67, 98, 82, 17, 124 };
    int size = sizeof(values) / sizeof(values[0]);
    
    for(int i = 0; i < size; i++)
    {
        append_item(array, values[i]);
    }

    print_values(array);
    
    free_array(array);
    
    return 0;
}
