#include <stdio.h>

int main(void)
{
    char *screen = "System Failure";
    char *override = "Access Granted";
    // create the lens
    // Are we using the double pointer becuase a pointer
    // of char can be seen as an array?

    // Note the pointer is pointing to the address of screen
    // Despite being a char and it should decay to a pointer
    // we need to get its address 
    char **lens = (char **)&screen;
    // change its value by assigning to address 0 the address
    // of override that should decay to a pointer 
    lens[0] = override;       
    
    printf("Screen shows: %s\n", screen); 
    
    return 0;
}
