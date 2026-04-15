#include <stdio.h>

void swap(int *a, int *b)
{
    printf("Swapping a = %d and b = %d\n", *a, *b);
    // temp gets the value from the pointer a by dereferencing the pointer a
    int temp = *a;
    // assign the value of the memory address of b to a by dereferencing
    *a = *b;
    // using dereferencing to assign the temp value to the pointer b
    *b = temp;
}

void show_swap(int a, int b)
{
    printf("a = %d\n", a);
    printf("b = %d\n", b);
}

void byte_inspector(int *p)
{
    printf("Byte Inspector of the integer %d \n", *p);

    // 1. We create a new pointer. We take 'p' and force it to be an unsigned char pointer.
    // 'unsigned char' is used for raw byte data so we don't deal with negative numbers.
    unsigned char *byte_ptr = (unsigned char *)p;

    int size = sizeof(*p);

    for (int i = 0; i < size; i++)
    {
        // Now, because byte_ptr is a char pointer, byte_ptr[i] steps forward exactly 1 byte at a time!
        // We use %02X to print the actual data inside the byte as a 2-digit Hexadecimal number.
        // We use %p to print the exact memory address of that specific byte.
        printf("Address: %p | Byte %d value: %02X\n", (void *)&byte_ptr[i], i, byte_ptr[i]);
    }
}

int main(void)
{
    int a = 10;
    int b = 4;
    int c = 14;

    int *p_a = &a;
    int *p_b = &b;

    swap(p_a, p_b);
    show_swap(a, b);

    byte_inspector(&c);
    return 0;
}
