#include <stdio.h>

/**
 * @brief Finds all pair of values in the array 
 * which sum is equal to the given target.
 * @param int numbers pointer
 * @param int size The size of the array 
 * @param int target
 * @return void
 */
void find_target(int *numbers, int size, int target)
{
    int next = 0;

    for(int left = 0; left < size; left++)
    {
        // check if next is still < size in order to reset it to 0.
        if(next < size)
        {
            /* To reduce the # of iterations I check whether
             * both pointers are greater than the target, if one
             * of those is, we know  the current iteration cannot
             * sum up to our target and we skip it. 
             */
            if(numbers[left] > target || numbers[next] > target)
            {
                left--;
                next++;
                continue;
            }
            
            if(numbers[left] + numbers[next] == target)
            {
                printf("%d + %d = %d\n", numbers[left], numbers[next], target);
                printf("indexes = %d & %d\n", left, next);
                printf("------------------\n");
            }

            left--;
            next++;
        }
        else
        {
            next = 0;
        }
    }
}

int main(void)
{
    int numbers[8] = {2, 7, 11, 9, 15, 2, 0, 9};
    int target = 9;
    int size = sizeof(numbers) / sizeof(numbers[0]);
    find_target(numbers, size, target);
    return 0;
}
