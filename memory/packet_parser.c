#include <stdio.h>

struct StandardHeader 
{
    unsigned char flags; // 1 byte
    unsigned int source_ip; // 4 bytes
    unsigned short port; // 2 bytes                    
}; // expecting a 5 bytes structs

struct PackedHeader
{
    unsigned char flags; // 1 byte
    unsigned int source_ip; // 4 bytes
    unsigned short port; // 2 bytes
} __attribute__((packed)); // force byte alignment




int main(void)
{
    printf("--- Network Packet Memory Alignment ---\n");

    struct StandardHeader sh;
    sh.flags = 's';
    sh.source_ip = 1234;
    sh.port = 80;

    struct PackedHeader ph;
    ph.flags = 'p';
    ph.source_ip = 2345;
    ph.port = 81;

    printf("SH size = %lu\n", sizeof(sh));
    printf("PH size = %lu\n", sizeof(ph));

    return 0;
}
