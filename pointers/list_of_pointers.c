#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Check the allocation
 * @param char list Double Pointer
 * @param int size The size of the array
 * @return void
 */
void check_row_allocation(char **p, int size)
{
    for(int i = 0; i < size; i++)
    {
        if(p[i] == NULL) exit(1);
    }
}

/**
 * @brief Free all the allocated memory
 * @param char list Double Pointer
 * @param int size The size of the array
 * @return void
 */
void free_ram(char **p, int size)
{
    for(int i = 0; i < size; i++)
        free(p[i]);

    free(p);
}

/**
 * @brief copy the value from the static matrix to the allocated list
 * @param char names Static Array
 * @param char list Double Pointer
 * @param int size The size of the array
 * @return void
 */
void copy_values(char names[][8], char **list, int size)
{
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < 8; j++)
        {
			// Copy the character first
            list[i][j] = names[i][j];
            
            if(names[i][j] == '\0') {
                break; // We hit the terminator. Stop copying this word!
            }
        }
    }
}

/**
 * @brief Print the values stored in the 2D array stored in the heap
 * @param char list Double Pointer
 * @param int size The size of the array
 * @return void
 */
void print_names(char **p, int size)
{
    for(int i = 0; i < size; i++)
        printf("Name at index %d is stored in the heap and it is  = %s\n", i,*(p+i));
}

int main(void)
{
    // 3 boxes and each can be max of 8 char long
    char names[3][8] = {"Ada", "Valerio", "C"};
    int arr_size = sizeof(names) / sizeof(names[0]);
    // cast into char **
    // give three main box in which inside we store the strings
    // I.E: [["Val"],["Leo"],["Grazia"]]
    // The size if of course the size of a char * pointer since
    // each box contains a char *
    char **roster = (char **)malloc(arr_size * sizeof(char *));
    if(roster == NULL) exit(1);
    // we also need to allocate space for the strings since 
    // those are still pointers, this time casting to a single char *
    // The size is based on how many char the string is made of plus the 
    // null terminator \0
    roster[0] = (char *)malloc((3+1) * sizeof(char)); // Ada
    roster[1] = (char *)malloc((7+1) * sizeof(char)); // Valerio
    roster[2] = (char *)malloc((1+1) * sizeof(char)); // C
    check_row_allocation(roster, arr_size);
    
    // simulate the work of strncpy
    copy_values(names, roster, arr_size);     
    // print the values we copied in the heap
    print_names(roster, arr_size);


    free_ram(roster, arr_size);
    return 0;
}
