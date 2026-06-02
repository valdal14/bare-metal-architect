#include <stdio.h>

int main(void)
{
    unsigned char array[48];
    
    void **lens1 = (void **)(array + 0);
    unsigned char **str1 = (unsigned char **)(array + 8);

    void **lens2 = (void **)(array + 16);
    unsigned char **str2 = (unsigned char **)(array + 24);

    void **lens3 = (void **)(array + 32); // The last node with no value and NULL 

    // connect the boxes and write the values
    int v1 = 7;
    int v2 = 14;

    lens1[0] = lens2;
    lens2[0] = lens3;
    lens3[0] = NULL;

    str1[0] = (unsigned char *)&v1;
    str2[0] = (unsigned char *)&v2;

    void **tracker = (void **)lens1;

    while(tracker != NULL)
    {
        void *address = (void *)tracker[0];
        printf("Block at address: %p\n", address);
        if(address != NULL) printf("Block value = %d\n", *(int *)(tracker[1]));
        tracker = address;
    }
    
    return 0;
}
