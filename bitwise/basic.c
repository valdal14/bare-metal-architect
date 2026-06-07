#include <stdio.h>
#include <stdint.h>

// AND (&) Operator
void and_for_condition(unsigned int value)
{
    unsigned int res = value & (1 << 0) ? 1 : 0;
    res == 1 ? printf("Found 1 at index 0\n") : printf("Did not find 1 at index 0\n");
}

void and(unsigned int op1, unsigned int op2)
{
    unsigned int res = (op1 & op2);
    printf("%d & %d = %d\n", op1, op2, res);
}

// OR (|) Operator
void or(unsigned int op1, unsigned int op2)
{
    unsigned int res = (op1 | op2);
    printf("%d | %d = %d\n", op1, op2, res);
}

void or_flip(unsigned int op1)
{
    /* 
     * NOTE: If the bit is already 1 does nothing.
     * unsigned int res = op1 | (1 << 0); does not 
     * change res to something else since the bit
     * at index 0 is already flipped
     */

    // Now let's try to flip the bit at index 1 
    // 9 =             0000 1001
    // 9 | (1 << 1)  = 0000 1011
    //
    // res = 2^3 + 2^1 + 2^0 -> 8 + 2 + 1 = 11
    unsigned int res = op1 | (1 << 1);
    printf("res =  %d\n", res); // 11   
}


// XOR (^) Operator
void xor(unsigned int op1, unsigned int op2)
{
    unsigned int res = op1 ^ op2;
    printf("%d ^ %d = %d\n", op1, op2, res); // 17
}

int main(void)
{
    // NOT (~) Operator
    unsigned int x = 9;
    unsigned int r = ~x;
    printf("r = %d\n", r); // -10
    
    // AND (&) Operator
    and_for_condition(x);
    and(9, 24); // 8
    
    // OR (|) Operator
    or(9, 24); // 25
    or_flip(9);

    // XOR (^) Operator
    xor(9, 24);

    return 0;
}
