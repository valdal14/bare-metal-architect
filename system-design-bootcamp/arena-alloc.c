#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    char *payload; // 8 bytes
    uint32_t size; // 4 bytes
    /**
     * bit 0 valid  
     * bit 2 red
     * bit 4 green
     * bit 8 blue
     */
    uint8_t flag; // 1 bytes 
    uint8_t padding[3]; // 3 bytes 
    // 16 bytes struct size 
} PixelPacket;

int main(void)
{
    return 0;
}
