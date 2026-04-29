#include <stdio.h>

int remove_duplicates(int *nums, int size)
{
    int sw = 0;
    
    for(int fs = 1; fs < size; fs++)
    {
        if(nums[fs] != nums[sw])
        {
            sw++; 
            nums[sw] = nums[fs];
        }
    }

    printf("--------------------\n");
    for(int i = 0; i < size; i++)
        printf("[%d]", nums[i]);

    printf("\n");

    return sw + 1;
}

int main(void)
{
    int nums[11] = {1, 2, 3, 4, 5, 5, 5, 5, 6, 6, 7};
    int size = sizeof(nums) / sizeof(nums[1]);
    int res = remove_duplicates(nums, size);
    printf("res = %d\n", res);
    return 0;
}
