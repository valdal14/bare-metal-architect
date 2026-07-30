#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_BALANCE 1000

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

typedef struct
{
    uint32_t total;
    uint8_t show_total;
} Balance;

Balance init(void)
{
    Balance balance;
    balance.total = 0;
    balance.show_total = 0;
    return balance;
}

void *increment(void *arg)
{
    Balance *balance = (Balance *)arg;

    pthread_mutex_lock(&lock);
    balance->total += 10;
    printf("balance = %d\n", balance->total);

    if(balance->total == MAX_BALANCE) 
    {
        balance->show_total = 1;
        pthread_cond_signal(&cond);
    }
    
    uint32_t *total = &balance->total; 
    pthread_mutex_unlock(&lock);

    return (void *)total;
}

void *show_total(void *arg)
{
    Balance *balance = (Balance *)arg;
    pthread_mutex_lock(&lock);
    while(balance->show_total == 0) pthread_cond_wait(&cond, &lock);
    uint32_t *total = &balance->total;
    pthread_mutex_unlock(&lock);
    printf("Getting the total balance...\n");
    sleep(1);
    printf("The total balance is: %d\n", *total);
    return (void *)total;
}

int main(void)
{
    Balance balance = init();
    printf("Init Balance: %d\n", balance.total);

    pthread_t threads[10];
    uint32_t *amt = NULL;

    for(int i = 0; i < 10; i++)
    {
        pthread_create(&threads[i], NULL, increment, (void *)&balance);
    }

    for(int i = 0; i < 10; i++)
    {
        pthread_join(threads[i], (void **)&amt);
    }

    printf("AMT = %d\n", *amt);

    return 0;
}
