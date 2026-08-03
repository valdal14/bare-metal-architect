#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

void swap_memory(void *a, void *b, size_t size, void(*swap)(uint8_t *a, uint8_t *b), void(*on_compl)(uint8_t *values, size_t size));
void swap(uint8_t*a, uint8_t *b);
void check_swap(uint8_t *values, size_t size);

int main(void)
{
    uint8_t a[4] = { 2, 4, 6, 8 };
    uint8_t b[4] = { 3, 5, 7, 9 };
    size_t size = sizeof(a) / sizeof(a[0]);
    swap_memory(a, b, size, swap, check_swap);
    return 0;
}

/**
 * @brief Prints out the result of the swap_memory function
 * @param uint8_t values pointer
 * @param size_t size
 * @return void
 */
void check_swap(uint8_t *values, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        printf("[%zu] = %d\n", i, *(values + i));
        sleep(1);
    }
    printf("------------\n");
}

/**
 * @brief Swaps two pointers 
 * @param uint8_t a pointer
 * @param uint8_t b pointer
 * @return void
 */
void swap(uint8_t *a, uint8_t *b)
{
    uint8_t temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief Swaps the memory of two pointers
 * @param void a pointer
 * @param void b pointer
 * @param size_t size
 * @param on_change callback (perform the single pointer location swap)
 * @param on_compl callback (show the results of the memory swap)
 * @return void
 */
void swap_memory(void *a, void *b, size_t size, void(*on_change)(uint8_t *a, uint8_t *b), void(*on_compl)(uint8_t *values, size_t size))
{
    uint8_t *va = (uint8_t *)a;
    uint8_t *vb = (uint8_t *)b;

    if(va == NULL || vb == NULL)
    {
        fprintf(stderr, "Error: NULL pointer passed to swap_memory\n");
        return;
    }

    for(size_t i = 0; i < size; i++) 
    {
        if(on_change) on_change(&va[i], &vb[i]);
    }

    if(on_compl)
    {
        on_compl(va, size);
        on_compl(vb, size);
    }
}
