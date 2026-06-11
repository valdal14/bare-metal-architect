#include <stdio.h>

unsigned int isPowerOfTwo(unsigned int n) 
{
    return (n > 0) && ((n & (n - 1)) == 0);
}

int main(void)
{
    for(unsigned int i = 0; i < 33; i++)
    {
        unsigned int n = i;
        unsigned int res = isPowerOfTwo(n);
        res == 1 ? printf("%d is power of 2\n", n) : printf("%d is not power of 2\n", n);
    }

    return 0;
}
