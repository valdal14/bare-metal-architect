#include <stdio.h>

void find_target(int *numbers, int size, int target)
{
    int left = 0;
    int right = size - 1;

    while(left < right)
    { 
        // 1. Calculate the sum exactly once per loop
        int current_sum = numbers[left] + numbers[right];
        
        // 2. The Perfect Match
        if(current_sum == target)
        {
            printf("%d + %d = %d\n", numbers[left], numbers[right], target);
            printf("indices %d & %d\n", left, right);
            return; // Exit the function entirely!
        }

        // 3. Magnitude Checks
        if(current_sum < target) 
        {
            left++;
        }
        else if(current_sum > target) 
        {
            right--;
        }
    }
}

int main(void)
{
    int numbers[8] = {-14, 1, 2, 7, 11, 12, 15, 27};
    int target = 3;
    int size = sizeof(numbers) / sizeof(numbers[0]);
    find_target(numbers, size, target);
    return 0;
}
