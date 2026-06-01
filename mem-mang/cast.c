#include <stdio.h>

int main(void)
{
    char c[5];
    c[0] = 'A';
    c[1] = 'B';
    c[2] = 'C';
    c[3] = 'D';
    c[4] = '\0';

    int *ptr = (int *)c;
    printf("%d\n", *ptr); // 1145258561

    char *s = (char *)c;
    printf("%c\n", *s); // A
    printf("%c\n", *(s + 1)); // B

    void *vp = (void *)c;
    printf("%s\n", (char *)vp + 1); // BCD
    printf("%s\n", (char *)vp); // ABCD

    
    return 0;
}
