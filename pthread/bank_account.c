#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex_account = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t log_condition = PTHREAD_COND_INITIALIZER;

typedef struct
{
    uint32_t balance;
} BankAccount;

typedef struct
{
    BankAccount *bank;
    uint32_t amount;
} DepositProcess;

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
    printf("Getting Balance...\n");
    sleep(1);
    uint32_t *balance = &bank_account->balance;
    return balance;
}

/**
 * @brief Deposits a new amount to the bank account
 * @param DepositProcess deposit
 * @return void
 */
void deposit(DepositProcess *deposit)
{
    printf("Depositing amount: %u\n", deposit->amount);
    sleep(2);
    uint32_t *current_balance = get_balance(deposit->bank);
    *current_balance += deposit->amount;
    printf("Deposit process completed\n");
}

/**
 * @brief Starts the Depositing process
 * @param BankAccount bank_account pointer
 * @param uint32_t amount
 * @return DepositProcess
 */
DepositProcess process_deposit(BankAccount *bank_account, uint32_t amount)
{
    DepositProcess dp;
    dp.bank = bank_account;
    dp.amount = amount;
    return dp;
}

int main(void)
{
    pthread_t *user1;
    pthread_t *user2;
    
    BankAccount *bank_account = NULL;
    open_account(&bank_account);
    printf("Account opened at address %p\n", bank_account);

    DepositProcess dp1 = process_deposit(bank_account, 540);
    deposit(&dp1);

    uint32_t *current_balance = get_balance(bank_account);
    printf("Current Balance = %u\n", *current_balance);
    printf("Current Balance = %u\n", bank_account->balance);


    return 0;
}
