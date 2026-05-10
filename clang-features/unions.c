#include <stdio.h>

typedef struct Packet
{
    char type;
    int payload;
} Packet;

typedef union RawData
{
    int number;
    char bytes[4];
} RawData;

int main(void)
{
    // 8 bytes = 1 char + 4 payload + 3 padding
    printf("sizeof Packet = %lu\n", sizeof(Packet));

    RawData rd;
    rd.number = 258;
    // 4 bytes = 4 * char (1)
    printf("sizeof RawData = %lu\n", sizeof(RawData));
    printf("bytes[0] = %d\n", rd.bytes[0]);    
    printf("bytes[1] = %d\n", rd.bytes[1]);    
    
    return 0;
}
