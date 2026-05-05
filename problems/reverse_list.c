#include <stdio.h>

/**
 * @brief Reverses an array of integers in-place using only pointer arithmetic.
 *        Strictly no bracket [] notation allowed.
 * @param arr Pointer to the first element of the array.
 * @param size The number of elements in the array.
 * @return void
 */
void reverse_array(int *arr, int size)
{
    int iteration = (int)(size / 2);
    int rewind = size - 1;

    for(int i = 0; i < iteration; i++)
    {
        int l = *(arr + i);
        int r = *(arr + rewind);
        *(arr + i) = r;
        *(arr + rewind) = l;
        rewind--;
    }
}

void print_array(int *arr, int size)
{
    for(int i = 0; i < size; i++)
        printf("[%d] = %d\n", i, arr[i]);
    printf("----------\n");
}

int main(void)
{
    int size = 9;
    int array[9] = {12,14,7,4,23,9,1,17,41};
    // original array 
    print_array(array, size);

    // reverse the array
    reverse_array(array, size);
    print_array(array, size);

    return 0;
}
