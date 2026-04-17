#include <stdio.h>

void print_decimal(int value)
{
    printf("decimal value: %d\n", value);
}

void print_hex(int value)
{
    printf("hex  value: 0x%02X\n", value);
}

void process_stream(int *data, int size, void (*action)(int chunk))
{
   for(int i = 0; i < size; i++)
       action(data[i]);
}

int main(void)
{
    int data[] = {10,20,30,255};
    int size = sizeof(data) / sizeof(data[0]);
    // pointer to array
    int *p_data = data;
    process_stream(p_data, size, print_decimal);
    printf("------------------------------\n");
    process_stream(p_data, size, print_hex);
    return 0;
}
