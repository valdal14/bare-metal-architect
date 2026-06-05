#include <stdio.h>

#define BLOCK 40

void init_list(unsigned char list[BLOCK], char *a, char *b)
{
    void **bk1 = (void **)(list + 0);
    unsigned char **v1 = (unsigned char **)(list + 8); 
    
    void **bk2 = (void **)(list + 16);
    unsigned char **v2 = (unsigned char **)(list + 24); 
    
    void **bk3 = (void **)(list + 32);

    // link the blocks 
    bk1[0] = bk2;
    bk2[0] = bk3;
    bk3[0] = NULL;

    // assign the values
    v1[0] = (unsigned char *)a;
    v2[0] = (unsigned char *)b;
}

void read_list(unsigned char list[BLOCK])
{
    // list head 
    void **head = (void **)list;

    while(head != NULL)
    {
        void *address = (void *)head[0];
        if(address == NULL) break;
        printf("address = %p\n", address);
        printf("val = %s\n", (unsigned char *)head[1]);
        head = address;
    }
}

int main(void)
{
    char a[6] = "Hello\0";
    char b[6] = "World\0";

    unsigned char list[BLOCK];
    init_list(list, a, b);
    read_list(list);
    return 0;
}
