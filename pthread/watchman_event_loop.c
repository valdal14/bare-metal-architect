#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t alarm_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t alert_cond = PTHREAD_COND_INITIALIZER;
uint8_t alarm_triggered = 0;

// int pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *mutex)
// Unlock the specified mutex, wait for a condition, and relock the mutex.

// int pthread_cond_signal(pthread_cond_t *cond)
// Unblock at least one of the threads blocked on the specified condition variable.

void *sensor_routine(void *arg)
{
    uint8_t *trigger = (uint8_t *)arg;
   
    sleep(2);
    pthread_mutex_lock(&alarm_lock);
    *trigger += 1;
    uint8_t current_state = *trigger;
    pthread_mutex_unlock(&alarm_lock);
    int res = pthread_cond_signal(&alert_cond);
    printf("Senson Routine ended with code: %d\n", res);
    current_state == 1 ? printf("Alarm ON\n") : printf("Alarm OFF\n");
    return trigger;
}

void *watchman_routine(void *arg)
{
    uint8_t *trigger = (uint8_t *)arg;
    printf("Watchman Routine started with code: %d\n", *trigger);
    
    pthread_mutex_lock(&alarm_lock);

    while(*trigger == 0)
    {
        pthread_cond_wait(&alert_cond, &alarm_lock);
    }
   
    if(*trigger == 1)
    {
        printf("Watchman: Intruder detected! Sounding the alarm!\n");
    }
    else
    {
        fprintf(stderr, "False alarm\n");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_unlock(&alarm_lock);

    return trigger;
}

void assert(uint8_t *t1, uint8_t *t2)
{
    *t1 + *t2 == 2 ? printf("Success Routine\n") : fprintf(stderr, "Routine Failed\n");
}

int main(void)
{
    pthread_t t_id1;
    pthread_create(&t_id1, NULL, watchman_routine, (void *)&alarm_triggered); 
    
    pthread_t t_id2;
    pthread_create(&t_id2, NULL, sensor_routine, (void *)&alarm_triggered); 

    uint8_t *t_watch = NULL;
    uint8_t *t_sens = NULL;
    
    pthread_join(t_id1, (void **)&t_watch);
    pthread_join(t_id2, (void **)&t_sens);

    assert(t_watch, t_sens); 
    
    return 0;
}

