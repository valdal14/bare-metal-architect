#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

typedef struct 
{
    /**
     * 8 bytes
     * 7 byte
     * 1 byte
     * -------
     * 16 bytes + 0 paddings = 16 bytes 
     */
    struct PipelineData *next;
    char cc_code[7];
    /**
     * bit 0 source
     * bit 2 destination
     * bit 4 processed
     */
    uint8_t metadata;

} PipelineData;


int main(void)
{
    return 0;
}
