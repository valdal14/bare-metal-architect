#include <stdio.h>
#include <stdlib.h>

#define EXEC  (1 << 0) // 1 bit 
#define WRITE (1 << 1) // 2 bit 
#define READ  (1 << 2) // 4 bit

/**
 * @brief Prints the current permission
 * @param int i The result of the Bitwise check 
 * @return void
 */
void print_result(int i)
{
    i == 1 ? printf("GRANTED\n") : printf("DENIED\n");
}

/**
 * @brief Grants a new permission using Bitwise OR
 * @param unsigned char current_perms
 * @param unsigned char new_perm
 * @return unsigned char (The updated permissions)
 */
unsigned char grant_permission(unsigned char current_perms, unsigned char new_perm)
{
    return current_perms | new_perm;
}

/**
 * @brief Checks if a permission exists using Bitwise AND
 * @param unsigned char current_perms
 * @param unsigned char target_perm
 * @return void
 */
void check_permission(unsigned char current_perms, unsigned char target_perm, void(*print_result)(int i))
{
    print_result((current_perms & target_perm) > 0);
}

int main(void)
{
    unsigned char user = 0;
    user = grant_permission(user, READ);
    user = grant_permission(user, WRITE);
    check_permission(user, READ, print_result);  
    check_permission(user, WRITE, print_result);  
    check_permission(user, EXEC, print_result);  
    return 0;
}
