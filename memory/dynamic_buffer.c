#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 5
#define MAX_STRING_LENGTH 32

void check_array_allocation(char **array)
{
    printf("--- Verifying memory allocation ---\n");

    if(array == NULL)
    {
        fprintf(stderr, "Memory Allocation Failed \n");
        exit(1);
    }
}

void check_char_allocation(char * c)
{
    printf("--- Checking single char allocation  ---\n");

    if(c == NULL)
    {
        fprintf(stderr, "Memory Allocation for char failed \n");
        exit(1);
    }
}


void read_data(char **data, int len)
{
    for(int i = 0; i < len; i++)
    {
        printf("%d = %s \n", i, *(data + i));
    }
}

void free_array(char **array)
{
    printf("--- Cleaning up memory ---\n");
    free(array);
}

void dynamic_memory_allocation()
{
    printf("--- Allocating memory ---\n");
    char **array = NULL;

    array = (char **)malloc(MAX_STRING_LENGTH * sizeof(char*));
    
    // check allocation
    check_array_allocation(array);
    // allocate memory for each string
    for(int i = 0; i < ARRAY_SIZE; i++)
    {
        array[i] = (char *)malloc((MAX_STRING_LENGTH + 1) * sizeof(char));
        // check single string allocation
        check_char_allocation(array[i]);
        // assign the values to the array
        sprintf(array[i], "Student-%d", i);
    }
    // read the data
	read_data(array, ARRAY_SIZE);
    free_array(array);
}

int main(void)
{
    // double pointer to store an array of strings
    dynamic_memory_allocation();
    return 0;
}

