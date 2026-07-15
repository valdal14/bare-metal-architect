#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>

_Atomic int counter = 0;

/**
 * @brief POSIX Threads callback
 * @param void arg point
 * @return void pointer 
 */
void *worker_loop(void *arg)
{
    // Ignoring the argument but supress the warning
    (void)arg;

    for(uint32_t i = 0; i < 1000000; i++)
    {
        int expected = counter;

        // The CAS Loop
        // atomic_compare_exchange_weak asks the CPU:
        // "Is 'counter' still equal to 'expected'? If yes, change it to 'expected + 1'."
        // If it fails, the CPU automatically updates our 'expected' variable to the new current value!
        while (!atomic_compare_exchange_weak(&counter, &expected, expected + 1))
        {
            // If the swap fails, 'expected' was magically updated.
            // The while loop restarts, evaluates (expected + 1) again, and retries!
        }
    }

    return (void *)&counter;
}

int main(void)
{
    pthread_t t[4];
    int *sum = NULL;

    for(uint8_t i = 0; i < 4; i++)
        pthread_create(&t[i], NULL, worker_loop, NULL);

    for(uint8_t i = 0; i < 4; i++)
        pthread_join(t[i], (void **)&sum);

    printf("Sum = %d\n", *sum);

    return 0;
}
