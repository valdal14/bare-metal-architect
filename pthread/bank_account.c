#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct
{
    uint32_t balance;
} BankAccount;

/**
 * @brief Checks the allocated objects
 * @param void account pointer
 * @return void
 */
void check_account(void *account)
{
    BankAccount *acc = (BankAccount *)account;
    
    if(acc == NULL)
    {
        fprintf(stderr, "Could not open a new bank account\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Opens a new bank account
 * @param BankAccount ba double pointer
 */
void open_account(BankAccount **ba)
{
    BankAccount *bank_account = (BankAccount *)calloc(1, sizeof(BankAccount));
    check_account(bank_account);
    bank_account->balance = 0;
    *ba = bank_account;
}

/**
 * @brief Returns the current balance 
 * @param BankAccount bank_account pointer
 * @return uint32_t
 */
uint32_t *get_balance(BankAccount *bank_account)
{
    uint32_t *balance = &bank_account->balance;
    return balance;
}

int main(void)
{
    BankAccount *bank_account = NULL;
    open_account(&bank_account);
    printf("Account opened at address %p\n", bank_account);
    uint32_t *current_balance = get_balance(bank_account);
    printf("Current Balance = %u\n", bank_account->balance);
    printf("Current Balance = %u\n", *current_balance);

    return 0;
}
