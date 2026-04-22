#include <stdio.h>
#include <stdlib.h>

void trap_redirect(int *p, int *new_target)
{
    // trying to point to the new target
    p = new_target;
}

void real_redirect(int **p, int *new_address)
{
    // dereferencing once to get to the pointer
    *p = new_address;
}

int main(void)
{
    int old_target = 10;
    int new_target = 99;
    int *ptr = &old_target;
    trap_redirect(ptr, &new_target);
    printf("ptr points to the value = %d\n", *ptr);

    // passing the address of the pointer itself
    real_redirect(&ptr, &new_target);
    printf("ptr points to the value = %d\n", *ptr);

    return 0;
}
