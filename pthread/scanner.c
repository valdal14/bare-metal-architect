#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

// Concurrency Primitives
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t state_changed = PTHREAD_COND_INITIALIZER;
// Macros 
#define TOGGLE(x) (1 << (x))
#define DEFAULT_MASK 0x00
#define POWER_BIT 0 
#define FREQU_BIT 1
#define READ_MASK 0x03 
#define MAX_POWER_LEVEL 100

// Scanner DS 
typedef struct 
{
    uint8_t power_level;
    uint8_t frequency_locked;
    /**
     * bit 0 powered 
     * bit 2 is_locked
     * I.E: Ready to read 0000 0011
     */
    uint8_t flags;
} Scanner;

// Concurrency Callbacks 
void *set_power(void *arg);
void *set_frequency(void *arg);
void *read_value(void *arg);

int main(void)
{
    Scanner scanner;
    scanner.power_level = 0;
    scanner.frequency_locked = 0;
    scanner.flags = DEFAULT_MASK;

    pthread_t power_t, frequency_t, reader_t;
    
    pthread_create(&power_t, NULL, set_power, (void *)&scanner);
    pthread_create(&frequency_t, NULL, set_frequency, (void *)&scanner);
    pthread_create(&reader_t, NULL, read_value, (void *)&scanner);

    pthread_join(power_t, NULL);
    pthread_join(frequency_t, NULL);
    pthread_join(reader_t, NULL);

    return 0;
}

/**
 * @brief Sets the power of the scanner to max level  
 * @param void arg pointer 
 * @return void pointer
 */
void *set_power(void *arg)
{
    Scanner *scanner= (Scanner *)arg;
    printf("Powering the Scanner on\n");
    sleep(2);
    pthread_mutex_lock(&lock);
    scanner->power_level = MAX_POWER_LEVEL;
    // toggle the bit power flag 
    scanner->flags |= TOGGLE(POWER_BIT);
    printf("Scanner is power on at level: %d\n", scanner->power_level);
    // send the signal of the power on
    pthread_cond_signal(&state_changed);
    pthread_mutex_unlock(&lock);
    return scanner;
}

/**
 * @brief Sets the frequency of the scanner
 * @param void arg pointer 
 * @return void pointer
 */
void *set_frequency(void *arg)
{
    Scanner *scanner = (Scanner *)arg;
    printf("Setting the Scanner's frequency on\n");
    sleep(3);
    pthread_mutex_lock(&lock);
    // wait until the scanner is powered on 
    while((scanner->flags & TOGGLE(POWER_BIT)) == 0) pthread_cond_wait(&state_changed, &lock);
    scanner->frequency_locked = 1;
    // toggle the bit frequency on 
    scanner->flags |= TOGGLE(FREQU_BIT);
    printf("Scanner's frequency is locked at: %d\n", scanner->frequency_locked);
    // send the signal of the frequency lock
    pthread_cond_signal(&state_changed);
    pthread_mutex_unlock(&lock);
    return scanner;
}

/**
 * @brief Reads the value from the scanner
 */
void *read_value(void *arg)
{
    Scanner *scanner = (Scanner *)arg;
    printf("Start reading from the Scanner\n");
    sleep(1);
    pthread_mutex_lock(&lock);
    // wait until the scanner is both power and frequency are on 
    while((scanner->flags & READ_MASK) != READ_MASK) pthread_cond_wait(&state_changed, &lock);
    printf("Scanner power: %d - Scanner frequency: %d\n", scanner->power_level, scanner->frequency_locked);
    pthread_mutex_unlock(&lock);
    return scanner;
}

