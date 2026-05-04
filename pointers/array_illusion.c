#include <stdio.h>


void change_value(int *p, int idx, int val)
{
    *(p + idx) = val;
}

void print_array(int *ptr, int size)
{
   for(int i = 0; i < size; i++)
       printf("[%d] = %d\n", i, *(ptr + i));
}

int main(void)
{
    int codes[5] = {10, 20, 30, 40, 50};
    int *ptr = codes;

    printf("[0] = %d\n", *ptr);
    printf("[3] = %d\n", *(ptr + 3));
    printf("-------------\n");
    
    change_value(ptr, 4, 99);
    print_array(ptr, 5);
    return 0;
}
