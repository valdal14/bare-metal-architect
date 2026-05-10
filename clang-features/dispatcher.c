#include <stdio.h>

#define ADD 0
#define SUB 1
/**
 * @brief Adds two numbers
 * @param int a 
 * @param int b 
 * @return int 
 */
int add(int a, int b)
{
    return a + b;
}

/**
 * @brief Subtracts two numbers
 * @param int a
 * @param int b
 * @return int
 */
int subtract(int a, int b)
{
    return a - b;
}

int main(void)
{
    // array of functions
    int (*operations[2])(int, int) = {add, subtract};
    // dispatch operation
    int operation_res = operations[SUB](14,4);
    
    printf("sub = %d\n", operation_res);
    return 0;
}
