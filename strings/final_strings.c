#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *MASTER_CODE = "OMEGA-99";

/**
 * @brief Receives a mutable string and replaces '-' with ' '
 * @param char buffer pointer
 * @return void
 */
void decrypt_local_buffer(char *buffer)
{
    int counter = 0;
    
    while(buffer[counter] != '\0')
    {
        if(buffer[counter] == '-')
        {
            buffer[counter] = ' ';
        }

        counter += 1;
    }

    printf("Decrypted Message = %s\n", buffer);
}

/**
 * @brief Mutate the given role based on the given new_role
 * @param char current_clearance double pointer
 * @param const char new_role read-only string
 * @return void
 */
void grant_clearance(char **current_clearance, const char *new_role)
{
   int new_role_size = strlen(new_role) + 1;
   char *role = (char *)calloc(1, new_role_size);
   if(role == NULL) exit(1);

   strncpy(role, new_role, new_role_size);
   // dereferencing it for mutation
   *current_clearance = role;
}

int main(void)
{
    // Mutable string in the stack
    char secret_message[] = "ACCESS-GRANTED-TO-SYSTEM";
    decrypt_local_buffer(secret_message);

    // read only 
    char *my_role = "GUEST";
    printf("Current Clearance = %s\n", my_role);
    grant_clearance(&my_role, "SUPER-ADMIN");
    // check the mutated string
    printf("Current Clearance = %s\n", my_role);

    free(my_role);
    return 0;
}
