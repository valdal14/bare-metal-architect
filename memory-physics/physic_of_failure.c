#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A function designed to allocate memory for our string
void allocate_message(char **message_ptr) 
{
    // We ask for 50 bytes on the heap
    *message_ptr = (char *)malloc(50);
    
    // We write a string into those 50 bytes
    strcpy(*message_ptr, "Hello from the Heap!");
}

void allocate_message_2(char **message_ptr)
{
    // get the block of memory we want to allocate from the message_ptr
    //char *ptr = &message_ptr[0][0];
    
    // we can do the same of the above by also doing
    // char *ptr = *message_ptr;
    
    // we can do the same of the above by also doing
    char *ptr = message_ptr[0];

    /**
     * In all of the above we are creating a new pointer ptr.
     * therefore we still need to tell the compiler, after we
     * allocated the space and copy the string that the message[0][0]
     * or *message_ptr or message_ptr[0] to points to that new pointer.
     */

    // allocate the space for the pointer that will store the string
    ptr = (char *)malloc(50);
    // We write a string into those 50 bytes
    strcpy(ptr, "Hello from the Valerio this time!");
    // assign it back 
    message_ptr[0] = ptr;
    // or by doing: 
    // *message_ptr = ptr;
}

int main(void) 
{
    // We create a pointer, currently pointing to nothing (0x0)
    char *my_string = NULL;

    // We pass our pointer to the function so it can be allocated
    allocate_message(&my_string);

    // We try to print the message
    printf("Message: %s\n", my_string);

    allocate_message_2(&my_string);

    printf("Message: %s\n", my_string);

    return 0;
}
