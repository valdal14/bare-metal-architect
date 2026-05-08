#include <stdio.h>
#include <stdlib.h>

int* get_bad_pointer(void)
{
    int secret = 42;
    return &secret;
}

int* get_good_pointer(void)
{
    int *secret = (int *)malloc(sizeof(int));
    if(secret == NULL) exit(1);
    // dereferencing the pointer to assign at 
    // that address the value 99 
    *secret = 99;
    return secret;
}

int main(void)
{
    int *sec1 = get_bad_pointer();
    printf("at address %p sec is %d\n", sec1, *sec1);

    int *sec2 = get_good_pointer();
    // dereferencing to get back the value
    printf("at address %p sec is %d\n", sec2, *sec2);

    free(sec2);
    return 0;
}
