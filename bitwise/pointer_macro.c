#include <stdio.h>

#define BIT(x) (1 << (x))

int main(void)
{
    // 0000 1001
    int n = 9;
    // Move bit at index 0 two places
    // 0000 1101 should give us 11 back
    n |= BIT(2);
    printf("%d\n", n);
    return 0;
}
