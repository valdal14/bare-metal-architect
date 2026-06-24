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
 * @brief Checks the returned value of a posix thread function
 * @param int thread_return_value
 * @return void
 */
void check_thread(int thread_return_value)
{
    if(thread_return_value != 0)
    {
        fprintf(stderr, "There was a problem with the thread, the app will terminate\n");
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
    // add a lock
    pthread_mutex_lock(&mutex_account);
    // update the shared balance state
    uint32_t *current_balance = get_balance(deposit->bank);
    *current_balance += deposit->amount;
    // remove the lock
    pthread_mutex_unlock(&mutex_account);
    printf("Deposit process completed\n");
}

/**
 * @brief Prepares the data needed to execute a new deposit operation
 * @param BankAccount bank_account pointer
 * @param uint32_t amount
 * @return DepositProcess
 */
DepositProcess prepare_for_deposit(BankAccount **bank_account, uint32_t amount)
{
    DepositProcess dp;
    dp.bank = *bank_account;
    dp.amount = amount;
    return dp;
}

/**
 * @brief Callback that internally calls the deposit process 
 * @param void arg pointer
 * @return void pointer
 */
void *deposit_exe(void *arg)
{
    DepositProcess *dp = (DepositProcess *)arg;
    
    if(dp == NULL)
    {
        fprintf(stderr, "Could not process the deposit operation\n");
        exit(EXIT_FAILURE);
    }
    // execute the deposit 
    deposit(dp);

    return NULL;
}

/**
 * @brief Callback that internally calls the get_balance
 * @param void arg pointer
 * @return void pointer
 */
void *user_balance(void *arg)
{
    DepositProcess *bank = (DepositProcess *)arg;
    check_account(bank->bank);
    pthread_mutex_lock(&mutex_account);
    uint32_t *balance = get_balance(bank->bank);
    pthread_mutex_unlock(&mutex_account);

    return (void *)balance;
}

int main(void)
{
    pthread_t user1;
    pthread_t user2;
    pthread_t u_balance;

    // Opening a new Bank Account   
    BankAccount *bank_account = NULL;
    open_account(&bank_account);
    printf("Account opened at address %p\n", bank_account);
    // Get ready to deposit
    DepositProcess dp1 = prepare_for_deposit(&bank_account, 240);
    DepositProcess dp2 = prepare_for_deposit(&bank_account, 360);
    // Check the thread return value and execute the deposit concurrently/in-parallel  
    check_thread(pthread_create(&user1, NULL, deposit_exe, (void *)&dp1));
    check_thread(pthread_create(&user2, NULL, deposit_exe, (void *)&dp2));
    // Joins the threads and get the balance 
    check_thread(pthread_join(user1, NULL));
    check_thread(pthread_join(user2, NULL));
    
    check_thread(pthread_create(&u_balance, NULL, user_balance, (void *)&dp1));
    
    uint32_t *new_balance = NULL;
    check_thread(pthread_join(u_balance, (void **)&new_balance));
    printf("Current Balance = %u\n", *new_balance);
    
    // clean up 
    free(bank_account);
    bank_account = NULL;
    return 0;
}
