#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 26

char *fill(int size)
{
    if(size > MAX_SIZE)
    {
        fprintf(stderr, "The size cannot be greater than %d", MAX_SIZE);
        exit(1);
    }

    char alphabet_utf8[] = u8"abcdefghijklmnopqrstuvwxyz";
    char *data = (char *)malloc(size * sizeof(char));
    
    if(data == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        exit(1);
    }

    for(int i = 0; i < size; i++)
    {
        data[i] = alphabet_utf8[i];
    }

    return data;
}

void print_list(char *list, int size)
{
    for(int i = 0; i < size; i++)
        // why here I do not need to dereference *list[i]
        // I thought to get to the value from a pointer we must
        // since this is not yet clear to me we need to review it
        // and work with theory and an example....
        printf("index: %d - value: = %c\n", i, list[i]);
}

int main(void)
{
    int size = 16;
    char *array = fill(size);
    print_list(array, size);
    free(array);

    char *poison_list = (char *)malloc(size * sizeof(char));
    print_list(poison_list, size);
    free(poison_list);

    char *cured_list = (char *)calloc(size, sizeof(char));
    print_list(cured_list, size);
    free(cured_list);

    return 0;
}
