#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    /**
     * a = 1 byte + 7 bytes padding (Offset 8)
     * b = 8 bytes (Offset 16)
     * c = 1 byte + 3 bytes padding (Offset 20)
     * d = 4 bytes (Final Size: 24)
     * ----------
     * 24 byte 
     */
    uint8_t a; 
    uint64_t b; 
    uint8_t c; 
    uint32_t d; 

} TypeA;

typedef struct
{
    /**
     * b = 8 byte 
     * d = 4 byte 
     * a = 1 byte 
     * c = 1 byte
     * ----------
     * 14 byte + 2 paddings
     */
    uint64_t b;
    uint32_t d;
    uint8_t a;
    uint8_t c;

} TypeB;

int main(void)
{
    TypeA ta;
    TypeB tb;
    printf("TypeA size = %lu\n", sizeof(ta));
    printf("TypeB size = %lu\n", sizeof(tb));
    return 0;
}
