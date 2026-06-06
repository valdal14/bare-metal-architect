#include <stdio.h>
#include <stdint.h>

typedef union
{
    uint32_t raw_data;
    uint8_t bytes[4];
} MemoryInspector;

int main(void)
{
    MemoryInspector xray;
    xray.raw_data = 0xDEADBEEF;

    for(int i = 0; i < 4; i++)
        printf("[%d] = %02X\n", i, xray.bytes[i]);

    return 0;
}
