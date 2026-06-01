#include <stdio.h>

int main(void)
{
    unsigned char sand[8];
    int target = 42;
   
    // Look at the sand as a place to store a pointer
    void **lens = (void **)sand;
    
    // Write THROUGH the lens
    // Notice there is no dereference (*) on target here! We want the address, not the 42
    lens[0] = &target;
    
    // Let's read it back out.
    // We look at the sand through the lens, and pull out the address we saved.
    int *retrieved_address = (int *)lens[0];

    printf("The value is: %d\n", *retrieved_address);

    return 0;
}
