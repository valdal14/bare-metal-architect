#include <stdio.h>

void engine_swap(long *core_a, long *core_b)
{
    long temp = *core_a;
    *core_a = *core_b;
    *core_b = temp;
}

int main(void)
{
    long core_a = 999;
    long core_b = 111;
    
    printf("Before: A=%ld, B=%ld\n", core_a, core_b);
    
    engine_swap(&core_a, &core_b);
    
    printf("After: A=%ld, B=%ld\n", core_a, core_b);
    
    return 0;
}
