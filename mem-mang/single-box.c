#include <stdio.h>

int main(void)
{
    unsigned char array[8];
    int pin = 1234;

    // Create a new pointer that points ad the address of pin 
    void *pin_ptr = &pin;

    // Create a writer void to write a memory address of pin 
    // into the array 
    void **writer = (void **)array;
    // writer[0] is the pointer at address 0 of the array 
    // so assigning to it the pointer gives me 
    // the address of pin 
    writer[0] = pin_ptr;
    
    // Then I create a tracker pointer to get back from the array at 
    // index 0 the to points at the address[0] of the array
    unsigned char **tracker = (unsigned char **)array;
    // Then I am casting the tracker to an int pointer and dereferencing
    // it to get the value
    printf("%d\n", *(int *)tracker[0]);

    return 0;
}
