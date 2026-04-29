#include <stdio.h>

/**
 * @brief Calculate the distance betweem two indexes
 * by first checking which of the two is greater 
 * in order to return a positive value
 * @param int idx1
 * @param int idx2
 * @return int 
 */
int calculate_distance(int idx1, int idx2)
{
    if(idx1 > idx2)
    {
        return idx1 - idx2;
    } 
    else
    {
        return idx2 - idx1;
    }
}

/**
 * @brief Calculates the water weight
 * @param int a The value of the first container 
 * @param int b The value of the second container
 * @return int
 */
int calculate_water_height(int a, int b)
{
    return a < b ? a : b;
}

/**
 * @brief Calculate the area 
 * @param int w Water weight
 * @param int d The distance
 */
int calculate_area(int w, int d)
{
    return w * d;
}

int max_area(int *heights, int size)
{
    int left = 0;
    int right = size - 1;
    int maxarea = 0;

    while(left < right)
    {
        int distance = calculate_distance(left, right);
        int wh = calculate_water_height(heights[left], heights[right]);
        int area = calculate_area(wh, distance);
        
        if(area > maxarea) maxarea = area;

        if(heights[left] > heights[right])
        {
            right--;
        }
        else
        {
            left++;
        }
    }

    return maxarea;
}

int main(void)
{
    int heights[9] = {1, 8, 6, 2, 5, 4, 8, 3, 7};
    int size = sizeof(heights) / sizeof(heights[0]);
    int area = max_area(heights, size);
    printf("Max Area = %d\n", area);
    return 0;
}
