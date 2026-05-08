#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Close a string from the source 
 * @param char source pointer
 * @return char pointer
 */
char* clone_string(char *source)
{
    int counter = 0;
    int str_size = 0;
    
    while(source[counter] != '\0') 
    {
        str_size += 1;
        counter++;
    }

    char *clone = (char *)malloc(str_size + 1);
    
    for(int i = 0; i < str_size; i++)
        *(clone + i) = *(source + i);

    clone[str_size] = '\0';

    return clone;
}


int main(void)
{
    char *c = "Systems Engineering";
    char *clone = clone_string(c);
    printf("Clone the string: %s\n", clone);
    free(clone);
    return 0;
}
