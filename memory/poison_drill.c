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
