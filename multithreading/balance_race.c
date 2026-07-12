#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#define TOGGLE(x) (1 << (x))
#define DEFAULT_OWNER_MASK 0x08
#define FULLNAME_MAX_SIZE 32
#define THREADS_NUMBER 10000

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct Owner
{
    char *fullname;
    /**
     * bit 0 = is locked (1 yes 0 no)
     * bit 2 = is premium (1 yes 0 no)
     * bit 4 = is negative (1 yes 0 no)
     * bit 8 = is active (1 yes 0 no)
     */
    uint8_t info;
} Owner;

typedef struct Account
{
    struct Owner *owner;
    uint32_t balance;
} Account;

typedef struct
{
    struct Account *account;
    uint32_t amount;
} DepositOperation;

/**
 * @brief Helper function used to create and return
 * a new Owner struct pointer 
 * @param const char fullname (max 32 chars)
 * @return Owner pointer
 */
Owner *create_owner(const char *fullname)
{
    Owner *owner = (Owner *)calloc(1, sizeof(Owner));

    if(owner == NULL) 
    {
        fprintf(stderr, "Could not create the new Owner\n");
        exit(EXIT_FAILURE);
    }

    uint8_t fullname_size = strlen(fullname) + 1;

    if(fullname_size > FULLNAME_MAX_SIZE)
    {
        fprintf(stderr, "Invalid account owner fullname\n");
        exit(EXIT_FAILURE);
    }
    
    owner->info = DEFAULT_OWNER_MASK;
    owner->fullname = (char *)malloc(sizeof(char) * fullname_size);

    if(owner->fullname == NULL)
    {
        fprintf(stderr, "Could not allocate space for the account owner fullname\n");
        exit(EXIT_FAILURE);
    }

    strncpy(owner->fullname, fullname, fullname_size);
    owner->fullname[fullname_size] = '\0';

    return owner;
}

/**
 * @brief Helper method used to show a confirmation 
 * of the new account creation
 * @param Account account pointer
 * @return void
 */
void account_creation(Account *account)
{
    printf("New Bank Account Created at address: %p\n", account);
    printf("Account Owner  : %s\n", account->owner->fullname);
    printf("Account Balance: %d EURO\n", account->balance);
    printf("--------------------------------------\n");
}

/**
 * @brief Creates a new Account associated with an Owner
 * @param Account account double pointer
 * @oaram const char fullname pointer (max 32 chars)
 * @return void
 */
void create_account(Account **account, const char *fullname, void(*confirmation)(Account *account))
{
    Account *new_account = (Account *)calloc(1, sizeof(Account));

    if(new_account == NULL)
    {
        fprintf(stderr, "Could not allocate space for the new account\n");
        exit(EXIT_FAILURE);
    }

    new_account->owner = create_owner(fullname);
    new_account->balance = 0;
    // confirmation callback
    confirmation(new_account);

    *account = new_account;
}

/**
 * @brief Adds a new amount to the current account balance
 * @param void arg pointer
 * @return void pointer
 */
void *deposit(void *arg)
{
     DepositOperation *dep = (DepositOperation *)arg;

     if(dep == NULL)
     {
         fprintf(stderr, "Could not perform deposit operation\n");
         exit(EXIT_FAILURE);
     }
     pthread_mutex_lock(&lock);
     dep->account->balance += dep->amount;
     pthread_mutex_unlock(&lock);
     return (void *)&dep->account->balance;
}

/**
 * @brief Cleans up the allocated memory 
 */
void cleanup_account(Account *account)
{
    free(account->owner->fullname);
    account = NULL;
}

int main(void)
{
    // Create Account
    Account *new_account = NULL;
    create_account(&new_account, "Valerio D'Alessio", account_creation);
    // Create the DepositOperation
    DepositOperation op1;
    op1.account = new_account;
    op1.amount = 1;

    // Spanw POSIX Threads 
    pthread_t t[THREADS_NUMBER];
    uint32_t *new_balance = NULL;

    for(uint16_t i = 0; i < THREADS_NUMBER; i++)
        pthread_create(&t[i], NULL, deposit, (void *)&op1);
    
    // Join the Threads
    for(uint16_t i = 0; i < THREADS_NUMBER; i++)
        pthread_join(t[i], (void **)&new_balance);


    printf("Returned Balance: %d\n", *new_balance);
    printf("Account Balance : %d\n", new_account->balance);
    cleanup_account(new_account);
    
    return 0;
}
