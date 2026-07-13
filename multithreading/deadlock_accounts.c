#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t lock_A = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t lock_B = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t completed = PTHREAD_COND_INITIALIZER;

typedef struct Account
{
    uint16_t balance;
} Account;

typedef struct
{
    struct Account *origin;
    struct Account *destination;
    uint16_t amount;
    uint8_t transaction_completed;
} Transfer; 

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


/**
 * @brief POSIX Thread callback used to transfer data
 * between Account A and B 
 * @param void arg pointer
 * @return void pointer
 */
void *transfer_a_to_b(void *arg)
{
    Transfer *trans = (Transfer *)arg;
    if(trans == NULL) return NULL;
    
    pthread_mutex_lock(&lock_A);
    sleep(1);
    pthread_mutex_lock(&lock_B);
    printf("A Balance: %d\n", trans->origin->balance);
    sleep(1);
    trans->origin->balance -= trans->amount;
    printf("A New Balance: %d\n", trans->origin->balance);
    sleep(1);
    printf("B New Balance: %d\n", trans->destination->balance);
    sleep(1);
    trans->destination->balance += trans->amount; 
    pthread_mutex_unlock(&lock_B);
    pthread_mutex_unlock(&lock_A);

    return NULL;
}

/**
 * @brief POSIX Thread callback used to transfer data
 * between Account B and A 
 * @param void arg pointer
 * @return void pointer
 */
void *transfer_b_to_a(void *arg)
{
    Transfer *trans = (Transfer *)arg;
    if(trans == NULL) return NULL;
    
    pthread_mutex_lock(&lock_A);
    sleep(1);
    pthread_mutex_lock(&lock_B);
    printf("B Balance: %d\n", trans->origin->balance);
    sleep(1);
    trans->origin->balance -= trans->amount;
    printf("B New Balance: %d\n", trans->origin->balance);
    sleep(1);
    printf("A New Balance: %d\n", trans->destination->balance);
    sleep(1);
    trans->destination->balance += trans->amount;
    // mark the full transaction as completed and send the signal
    trans->transaction_completed = 1;
    pthread_mutex_unlock(&lock_B);
    pthread_mutex_unlock(&lock_A);

    pthread_cond_signal(&completed);

    return NULL;
}

/**
 * @brief POSIX Thread callback used to check both Account
 * balances once the transaction is completed.
 * @param void arg pointer
 * @return void pointer
 */
void *check_balance(void *arg)
{
    Transfer *trans = (Transfer *)arg;
    if(trans == NULL) return NULL;

    pthread_mutex_lock(&lock_B);

    while(trans->transaction_completed == 0) 
    {
        pthread_cond_wait(&completed, &lock_B);
    }

    printf("Origin Balance     : %d\n", trans->origin->balance);
    printf("Destination Balance: %d\n", trans->destination->balance);
    
    pthread_mutex_unlock(&lock_B);
    return NULL;
}

/**
 * @brief Cleans up the allocated memory
 * @param Transfer transfer pointer
 * @return void
 */
void clean(Transfer *transfer)
{
    free(transfer->origin);
    free(transfer->destination);
}

int main(void)
{
    pthread_t ta, tb, check;
    Account *acc1 = NULL;
    Account *acc2 = NULL;
    init(&acc1, 500);
    init(&acc2, 400);
    printf("Acc1 allocated at address %p\n", acc1);
    printf("Acc2 allocated at address %p\n", acc2);

    Transfer transfer1;
    transfer1.origin = acc1;
    transfer1.destination = acc2;
    transfer1.amount = 250;
    transfer1.transaction_completed = 0;

    Transfer transfer2;
    transfer2.origin = acc2;
    transfer2.destination = acc1;
    transfer2.amount = 300;
    transfer2.transaction_completed = 0;

    pthread_create(&ta, NULL, transfer_a_to_b, (void *)&transfer1);
    pthread_create(&tb, NULL, transfer_b_to_a, (void *)&transfer2);
    pthread_create(&check, NULL, check_balance, (void *)&transfer2);

    pthread_join(ta, NULL);
    pthread_join(tb, NULL);
    pthread_join(check, NULL);

    clean(&transfer1);

    return 0;
}
