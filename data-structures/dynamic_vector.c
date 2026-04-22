#include <stdio.h>
#include <stdlib.h>

struct Vector
{
    int *data;
    int size;
    int capacity;
};

/**
 * @brief Factory method used to create the struct Vector
 * @param int initial_capacity The capacity of the data pointer
 * @return struct Vector
 */
struct Vector* vector_create(int initial_capacity)
{
    struct Vector *vector = (struct Vector *)calloc(1, sizeof(struct Vector));
    
    if(vector == NULL)
    {
        fprintf(stderr, "Could not allocate space for the new Vector\n");
        exit(1);
    }

    vector->data = (int *)calloc(initial_capacity, sizeof(int));
    vector->size = 0;
    vector->capacity = initial_capacity;
    return vector;
}


/**
 * @brief Resize the data stored in the heap
 * @param struct Vector Pointer
 * @return void
 */
void resize_if_needed(struct Vector *v)
{
    if(v->size == v->capacity)
    {
        v->capacity = v->capacity * 2;
        int *temp = (int *)realloc(v->data, v->capacity * sizeof(int));
        
        if(temp == NULL)
        {
            fprintf(stderr, "Could not resize the data\n");
            exit(1);
        }
        
        v->data = temp;
    }
}

/**
 * @brief Push a new value into the Vector stored in the heap 
 * @param struct Vector Pointer
 * @param int value Tne new value we want to push
 * @return void
 */
void vector_push(struct Vector *v, int value)
{
    resize_if_needed(v);
    // move the pointer and assign the new value
    v->data[v->size] = value;
    // increment the size by 1
    v->size = v->size + 1;
}

/**
 * @brief Get the value stored by a given index
 * @param struct Vector Pointer
 * @param int index The index of the value we want to get back
 * from the Vector
 * @return int
 */
int vector_get(struct Vector *v, int index)
{
    if(index < 0 || index >= v->size)
    {
        fprintf(stderr, "Index OutOfBound Error\n");
        exit(1);
    }

    return v->data[index];
}

/**
 * @brief Free the previous allocated memory
 * @param struct Vector Pointer
 * @return void
 */
void vector_free(struct Vector *v)
{
    free(v->data);
    free(v);
}

/**
 * @brief Prints out the stats of the Vector
 * @param struct Vector Pointer
 * @return void
 */
void print_vector_stats(struct Vector *v)
{
    printf("----------------------------\n");
    printf("Vector Current Size     = %d\n", v->size);
    printf("Vector Current Capacity = %d\n", v->capacity);
    printf("----------------------------\n");

    for(int i = 0; i < v->size; i++)
    {
        printf("At index %d the value stored is = %d\n", i, v->data[i]);
    }

    printf("----------------------------\n");
}

int main(void)
{
    struct Vector *v = vector_create(2);
    vector_push(v, 14);
    vector_push(v, 7);
    vector_push(v, 21);
    vector_push(v, 19);
    vector_push(v, 31);
    vector_push(v, 77);
    
    // search
    int index = 4;
    int val = vector_get(v, index);
    printf("Search at index %d found the value %d\n", index, val);

    // iterate
    print_vector_stats(v); 

    vector_free(v);
    return 0;
}
