#include <stdio.h>
#include <stdint.h>

#define R_PERM 0x01
#define W_PERM 0x02
#define E_PERM 0x04

void grant_rw(uint8_t *status)
{
    uint8_t change_status = 0;
    // faster way
    change_status |= (R_PERM | W_PERM);
    //change_status = change_status | R_PERM;
    //change_status = change_status | W_PERM;
    *status = change_status;
}

int main(void)
{
    // All switches are off
    uint8_t user_status = 0;
    grant_rw(&user_status);

    if(user_status & R_PERM) printf("Read Permission Granted\n");
    if(user_status & W_PERM) printf("Write Permission Granted\n");
    if(user_status & E_PERM) printf("Execute Permission Granted\n");
    
    return 0;
}
