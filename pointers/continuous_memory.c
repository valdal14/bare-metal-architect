#include <stdio.h>

void crack_vault(int *ptr)
{
    for(int i = 0; i < 3; i++)
    {
        if(i == 1) 
        {
            // Mutating the pointer at specific index
            *(ptr + i) = *(ptr + i) *5;
            printf("Mutation %d\n", *(ptr + 1)); 
        }
        else
        {
            printf("%d\n", *(ptr + i));
        }
    }
}

int main(void)
{
    int vault[3] = {10, 20, 30};
    // passing the array directly no need to get the address
    // of using using & since it is already a pointer to the 
    // first value of it.
    crack_vault(vault);
    return 0;
}

