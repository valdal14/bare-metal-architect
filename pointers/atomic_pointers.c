#include <stdio.h>

void double_it(int *num)
{
    // step 1 - get to the pointer by derefering it (before assignment)
    // step 2 - get to the pointer by dereferencing it (after assignment)
    // step 3 - multiple the value got from step 2 and multiply it by 2
    *num = *num * 2;
}

int main(void)
{ 
    int num = 2;
    
    for(int i = 0; i < 10; i++)
    {
        double_it(&num);
        printf("num = %d\n", num);
    }

    return 0;
}
