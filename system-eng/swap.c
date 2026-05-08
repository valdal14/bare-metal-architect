#include <stdio.h>

void swap(int *a, int *b)
{
   int temp = *a;
   *a = *b;
   *b = temp;
}

int main(void)
{
    int a, b;
    a = 10;
    b = 99;
    printf("a = %d\n", a);
    printf("b = %d\n", b);

    swap(&a, &b);
    printf("Swap completed: a = %d & b = %d\n", a, b);

    return 0;
}
