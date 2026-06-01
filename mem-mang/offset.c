#include <stdio.h>

int main(void)
{
    unsigned char sand[16]; // 16 bytes of empty sand
    long vault_password = 8675309;
    // here I have used a void pointer for testing 
    void **lens = (void **)sand;

    lens[1] = (unsigned char *)&vault_password;
   
    long *pwd = (long *)lens[1];

    printf("Password = %ld\n", *pwd);

    // Here another test but adding an unsigned char using the char_lens
    unsigned char **char_lens = (unsigned char **)sand;
    char_lens[0] = (unsigned char *)"!P@ssW0rd!\0";

    unsigned char *new_pwd = (unsigned char *)char_lens[0];
 
    printf("New Password = %s\n", new_pwd);

    return 0;
}
