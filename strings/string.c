#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// read-only space
const char *SYS_MSG = "System Booting...";

// arrays decay to pointers, so this accepts arrays
void make_uppercase(char *text)
{
    int i = 0;
    
    // Keep looping as long as the current character is NOT the null terminator
    while (text[i] != '\0') 
    {
        // Check if the character is lowercase
        if (text[i] >= 'a' && text[i] <= 'z') 
        {
            text[i] = text[i] - 32;
        }
        i++; // Move to the next index
    }
}


char* create_greeting(const char *name)
{
    // 7 bytes for "Hello, " + length of name + 1 for '\0'
    int total_size = 7 + strlen(name) + 1; 

    char *greet_name = (char *)malloc(total_size);
    if(greet_name == NULL) exit(1);

    // "Print" the formatted string directly into the Heap memory!
    sprintf(greet_name, "Hello, %s", name);
    
    return greet_name;
}

int main(void)
{
    printf("%s\n", SYS_MSG);
   
    // Stack and decay to an array
    char greet[14] = "Hello Valerio";
    make_uppercase(greet);
    printf("%s\n", greet);

    char *greeting = create_greeting("Grace");
    printf("%s\n", greeting);

    free(greeting);

    return 0;
}
