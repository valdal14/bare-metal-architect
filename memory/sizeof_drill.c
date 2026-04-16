#include <stdio.h>

struct Struct 
{
    int data[100];
    double timestamp;
};

int main(void)
{
    char c = 'c';
    int i = 14;
    double d = 14.7;
    struct Struct s;

    printf("size of char in bytes: %zu\n", sizeof(c)); 
    printf("size of int in bytes: %zu\n", sizeof(i));
    printf("size of double in bytes: %zu\n", sizeof(d));
    printf("size of Struct in bytes: %zu\n", sizeof(s));
    
    char *cp = &c;
    int *ip = &i;
    double *dp = &d;
    struct Struct *sp = &s;
    
    printf("size of char pointer in bytes: %zu\n", sizeof(cp)); 
    printf("size of int pointer in bytes: %zu\n", sizeof(ip)); 
    printf("size of double pointer in bytes: %zu\n", sizeof(dp)); 
    printf("size of struct pointer in bytes: %zu\n", sizeof(sp)); 



    return 0;
}
