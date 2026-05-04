#include <stdio.h>

// rewire heist_target so that it points to vault_b
void redirect_target(int **p, int *target)
{
    printf("value ay *p = %d\n", *p[0]);
    // redirect the pointer 
    *p = target; 
}

int main(void)
{
    int vault_a = 100;
    int vault_b = 200;
    // this pointer points to vault_a
    int *heist_target = &vault_a;
    printf("[before] heist_target value = %d\n", *heist_target);
    redirect_target(&heist_target, &vault_b);
    printf("[after] heist_target value = %d\n", *heist_target);
    return 0;
}
