#include <stdio.h>
#include <stdint.h>

void swap_memory(void *a, void *b, size_t size, void(*on_compl)(uint8_t *values, size_t size));
void check_swap(uint8_t *values, size_t size);

int main(void)
{
    uint8_t a[4] = { 2, 4, 6, 8 };
    uint8_t b[4] = { 3, 5, 7, 9 };
    size_t size = sizeof(a) / sizeof(a[0]);
    swap_memory(a, b, size, check_swap);
    return 0;
}

void check_swap(uint8_t *values, size_t size)
{
    for(uint8_t i = 0; i < size; i++)
        printf("A[i] = %d\n", *(values + i));
}

void swap_memory(void *a, void *b, size_t size, void(*on_compl)(uint8_t *values, size_t size))
{
    uint8_t *va = (uint8_t *)a;
    uint8_t *vb = (uint8_t *)b;
    uint8_t *temp = NULL;

    if(va == NULL || vb == NULL)
    {
        fprintf(stderr, "Casting to uint8_t procuded an error\n");
        return;
    }

    for(uint8_t i = 0; i < size; i++)
    {
        temp[i] = *(va + i);
        va[i] = vb[i];
        vb[i] = temp[i];
    }

    on_compl(va, size);

}
