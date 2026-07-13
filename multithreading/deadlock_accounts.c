#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

typedef struct
{
    uint16_t balance;
} Account;

/**
 * @brief Inits a new Account
 * @param Account account double pointer
 * @param uint16_t amount 
 * @return void
 */
void init(Account **account, uint16_t amount)
{
    Account *new_account = (Account *)malloc(sizeof(Account));

    if(new_account == NULL)
    {
        fprintf(stderr, "Could not allocate space for the new account\n");
        exit(EXIT_FAILURE);
    }

    new_account->balance = amount;
    *account = new_account;
}

int main(void)
{
    Account *acc1 = NULL;
    Account *acc2 = NULL;
    init(&acc1, 500);
    init(&acc2, 400);
    printf("Acc1 allocated at address %p\n", acc1);
    printf("Acc2 allocated at address %p\n", acc2);
    return 0;
}
