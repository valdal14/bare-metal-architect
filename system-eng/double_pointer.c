#include <stdio.h>
#include <stdlib.h>

void create_number(int **ptr_ref)
{
    int *num = (int *)malloc(sizeof(int));
    if(num == NULL) exit(1);
    *num = 777;

    *ptr_ref = num;
}

int main(void)
{
    int *my_num = NULL;
    create_number(&my_num);
    printf("Number created is %d\n", *my_num);
    free(my_num);
    return 0;
}
