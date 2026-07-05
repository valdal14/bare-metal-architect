#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>    // Required for timespec
#include <errno.h>   // Required for ETIMEDOUT

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t data_ready = PTHREAD_COND_INITIALIZER;
int is_ready = 0;

void *api_worker(void *arg) 
{
    printf("[Worker] Requesting data from downstream... waiting max 3 seconds.\n");

    // Calculate the ABSOLUTE time we want to wake up
    struct timespec timeout_time;
    clock_gettime(CLOCK_REALTIME, &timeout_time); // Get current system clock time
    timeout_time.tv_sec += 3;                     // Add 3 seconds to the current time

    pthread_mutex_lock(&lock);

    int rc = 0;
    // The loop now checks BOTH the boolean state AND the return code
    while(is_ready == 0 && rc == 0) 
    {
        rc = pthread_cond_timedwait(&data_ready, &lock, &timeout_time);
    }

    // Evaluate why we woke up
    if(rc == ETIMEDOUT) 
    {
        printf("[Worker] FATAL: 504 Gateway Timeout. System is unresponsive. Aborting.\n");
    } 
    else 
    {
        printf("[Worker] SUCCESS: Data received safely before the timeout.\n");
    }

    pthread_mutex_unlock(&lock);
    return NULL;
}

int main(void) 
{
    pthread_t worker;
    pthread_create(&worker, NULL, api_worker, NULL);

    // --- THE TEST MECHANISM ---
    // Change this to sleep(1) to see the SUCCESS message.
    // Leave it at sleep(5) to see the ETIMEDOUT Circuit Breaker trigger.
    sleep(5); 

    pthread_mutex_lock(&lock);
    is_ready = 1;
    pthread_cond_signal(&data_ready);
    pthread_mutex_unlock(&lock);
    
    printf("[Main] System finally provided the data.\n");

    pthread_join(worker, NULL);
    return 0;
}
