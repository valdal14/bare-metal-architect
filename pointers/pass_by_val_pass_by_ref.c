#include <stdio.h>

void attempt_corruption(int x)
{
    x = 99;
    printf("[attempt_corruption] x = %d\n", x);
}

void attempt_corruption_with_pointer(int *x)
{
    // derefetencing the poiter to change the value of x and print it from 
    // withing the function
    *x = 99;
    printf("[attempt_corruption_with_pointer] x = %d\n", *x);
}

int main(void)
{
    int x = 10;
    attempt_corruption(x);
    printf("[after attempt_corruption] x = %d\n", x);
    
    attempt_corruption_with_pointer(&x);
    printf("[after attempt_corruption_with_pointer] x = %d\n", x);

    return 0;
}
