#include <stdio.h>
#include <stdlib.h>

#define BASE_RULE 14

/**
 * @brief Check the list row allocation of the int pointers
 * @param int list The double pointer
 * @param int The size of the grade's list
 * @return void
 */
void check_ints_alloc(int **list, int size)
{
    for(int i = 0; i < size; i++)
        if(list[i] == NULL) exit(1);
}   

/**
 * @brief Assign the grades to the students
 * @param int grades The double pointer list
 * @param int sizes The list with the 3 grade book grades
 * @size int The size of the grade's list
 * @return void
 */
void assign_grades(int **grades, int sizes[3], int size)
{
    for(int i = 0; i < size; i++)
    {
        int inner_size = sizes[i];
        
        for(int j = 0; j < inner_size; j++)
        {
            grades[i][j] = j + BASE_RULE;
        }

    }
}

/**
 * @brief Prints the grades
 * @param int grades The double pointer list
 * @param int sizes The list with the 3 grade book grades
 * @size int The size of the grade's list
 * @return void
 */
void print_grades(int **grades, int sizes[3], int size)
{
    for(int i = 0; i < size; i++)
    {
        int inner_size = sizes[i];

        for(int j = 0; j < inner_size; j++)
            printf("Student at index %d got grade = %d\n", i, grades[i][j]);

        printf("---------------------------------\n");
    }
}

/**
 * @brief Free the previous allocated memeory
 * @param int grades The double pointer list
 * @size int The size of the grade's list
 * @return void
 */
void free_ram(int **grades, int size)
{
    for(int i = 0; i < size; i++)
        free(grades[i]);

    free(grades);
}

int main(void)
{
    int sizes[3] = {2, 4, 1};
    const int size = sizeof(sizes) / sizeof(sizes[0]);

    int **grade_book = (int **)malloc(size * sizeof(int *));
    if(grade_book == NULL) exit(1);

    grade_book[0] = (int *)malloc(2 * sizeof(int));
    grade_book[1] = (int *)malloc(4 * sizeof(int));
    grade_book[2] = (int *)malloc(1 * sizeof(int));
    
    check_ints_alloc(grade_book, size);

    assign_grades(grade_book, sizes, size);  
    
    print_grades(grade_book, sizes, size);
    
    free_ram(grade_book, size);
    
    return 0;
}
