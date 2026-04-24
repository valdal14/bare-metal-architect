#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Roster 
{
    char *title;          // Pointer (Read-Only literal or Heap)
    char code[6];         // Fixed Stack Array (e.g., "CS10")
    char names[3][15];    // 2D Stack Matrix (3 names, max 14 chars each)
};

int main(void)
{
    struct Roster roster;

    // Char Pointer Assignment ---------------------
    roster.title = "Pointer & Arrays";
    printf("Title %s\n", roster.title);
    
    // Fixed Stack Array ----------------------------
    /**
     * To assign value to a Fixed Array in the stack
     * we can use strncpy or iteration like i did below
     * to mutate the value or roster.code. We can also
     * rely on the pointer arithmetic in the last for loop
     * */
    
    // case 1
	strncpy(roster.code, "CS150", sizeof(roster.code) - 1);
	roster.code[sizeof(roster.code) - 1] = '\0';
	printf("Class: %s\n", roster.code);

    
    // case 2
    char *class = {"CS180"};

    for(int i = 0; i < 5; i++)
        roster.code[i] = class[i];
    
    printf("Class %s\n", roster.code);

    // case 3
	char *new_class = "CS190";
	
	for(int i = 0; i < 5; i++) 
	{
		roster.code[i] = *(new_class + i);
	}
	
	roster.code[5] = '\0'; // Manually cap it!
	printf("Class Update: %s\n", roster.code);

    // 2D Stack Matrix ---------------------------------
    char students[3][15] = {"Nash", "Turing", "Rickie"};
     
    /**
     * To get to the value of the string itself
     * we do not need to reference the matrix using [0][1]
     * */
	for(int i = 0; i < 3; i++)
	{
		// Copy using the max size of the destination row (15), minus 1 for safety
	    strncpy(roster.names[i], students[i], sizeof(roster.names[i]) - 1);
        
	    // Manually cap the row
	    roster.names[i][sizeof(roster.names[i]) - 1] = '\0';
        
	    printf("Student's name = %s\n", roster.names[i]);
	}


    return 0;
}
