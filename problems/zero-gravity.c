#include <stdio.h>


void move_zeroes(int *nums, int size)
{
    int sw = 0;

    for(int fs = 0; fs < size; fs++)
    {
        if(nums[fs] != 0)
        {
            nums[sw] = nums[fs];
            sw++;
        }
    }

    for(int i = sw; i < size; i++)
        nums[i] = 0;

    for(int i = 0; i < size; i++)
        printf("[%d]", nums[i]);
    printf("\n");
}


int main(void)
{
    int nums[5] = {0, 1, 0, 3, 12};
    int size = sizeof(nums) / sizeof(nums[0]);
    move_zeroes(nums, size);
    return 0;
}
