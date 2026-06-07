#include <stdio.h>
#include <stdint.h>

#define MASK2 0x01
#define MASK3 0x02

uint8_t flip_bits(uint8_t state) {
    
    uint8_t mask_idx2 = MASK2;
    uint8_t mask_idx3 = MASK3;
    
    state ^= (1 << mask_idx2);
    state ^= (1 << mask_idx3);

    uint8_t result = state; 
    
    return result;
}

int main(void)
{
    uint8_t res = flip_bits(11);
    printf("%d\n", res);
    return 0;
}
