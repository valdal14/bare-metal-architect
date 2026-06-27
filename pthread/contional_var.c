#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#define BIT(x) (1 << (x))
#define DEFAULT_MASK 0x00
#define READY_MASK   0x0B 
#define CLEAN_BIT    0 
#define FILL_BIT     1 
#define OPEN_BIT     3    
#define FILL_VALUE   4 
#define CLEAN_VALUE  3 

// Concurrency Primitives
pthread_cond_t is_clean = PTHREAD_COND_INITIALIZER;
pthread_cond_t is_full = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct 
{
    /**
     * bit 0 = Clean (Value 1)
     * bit 1 = Full  (Value 2)
     * bit 3 = Open  (Value 8)
     * Target state: 1011 = 11 base 10 (0x0B)
     */
    uint8_t state;
} Pool;

/**
 * @brief Simulates the wait for the cleaning process.
 * @return void
 */
void start_cleaning(void)
{
    for(int i = 0; i < CLEAN_VALUE; i++)
    {
        printf("\t[Cleaner] Scrubbing... %d/%d\n", i + 1, CLEAN_VALUE);
        sleep(1);
    }
}

/**
 * @brief Simulates the wait for the filling process.
 * @return void
 */
void start_filling(void)
{
    for(int i = 0; i < FILL_VALUE; i++)
    {
        printf("\t[Filler] Pumping water... %d/%d\n", i + 1, FILL_VALUE);
        sleep(1);
    }
}

/**
 * @brief Checks pointer allocation.
 * @param Pool *pool Pointer to the allocated pool struct.
 * @return void
 */
void check_pool(Pool *pool)
{
    if(pool == NULL)
    {
        fprintf(stderr, "Could not allocate space for the Pool\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Initializes the Pool with default values.
 * @param Pool **pool Double pointer to the pool struct.
 * @return void
 */
void init_pool(Pool **pool)
{
    Pool *p = (Pool *)calloc(1, sizeof(Pool));
    check_pool(p); 
    p->state = DEFAULT_MASK;
    *pool = p; 
}

/**
 * @brief Cleans the Pool and broadcasts the clean state.
 * @param void *arg Pointer to the Pool struct.
 * @return void* Returns NULL upon thread completion.
 */
void *clean(void *arg)
{
    Pool *pool = (Pool *)arg;

    printf("[Cleaner] Pool is closed. Cleaning in progress...\n");
    start_cleaning();

    // Lock, update state, and unlock
    pthread_mutex_lock(&mutex);
    pool->state |= BIT(CLEAN_BIT);
    printf("[Cleaner] Done! State is now: %d (0x%02X)\n", pool->state, pool->state);
    pthread_mutex_unlock(&mutex);

    // Broadcast to wake up anyone waiting for the pool to be clean
    pthread_cond_broadcast(&is_clean);

    return NULL;
}

/**
 * @brief Fills the Pool after waiting for it to be cleaned.
 * @param void *arg Pointer to the Pool struct.
 * @return void* Returns NULL upon thread completion.
 */
void *fill(void *arg)
{
    Pool *pool = (Pool *)arg;

    // Lock to check the state
    pthread_mutex_lock(&mutex);

    // Wait until the CLEAN_BIT is flipped to 1
    while((pool->state & BIT(CLEAN_BIT)) == 0)
    {
        printf("[Filler] Waiting for pool to be cleaned...\n");
        pthread_cond_wait(&is_clean, &mutex);
    }
    
    // Unlock so we don't hold the mutex while filling!
    pthread_mutex_unlock(&mutex);

    printf("[Filler] Pool is clean! Start filling the pool...\n");
    start_filling();

    // Lock, update state, and unlock
    pthread_mutex_lock(&mutex);
    pool->state |= BIT(FILL_BIT);    
    printf("[Filler] Done! State is now: %d (0x%02X)\n", pool->state, pool->state);
    pthread_mutex_unlock(&mutex);
    
    // Broadcast to wake up anyone waiting for the pool to be full
    pthread_cond_broadcast(&is_full);

    return NULL;
}

/**
 * @brief Opens the Pool after verifying it is both clean and filled.
 * @param void *arg Pointer to the Pool struct.
 * @return void* Returns NULL upon thread completion.
 */
void *open_pool(void *arg)
{
    Pool *pool = (Pool *)arg;
    
    // Lock the door to check the state
    pthread_mutex_lock(&mutex);

    // Wait until BOTH Clean and Fill bits are set
    // (CLEAN_BIT | FILL_BIT) resolves to 3 (Binary 0011)
    while((pool->state & (BIT(CLEAN_BIT) | BIT(FILL_BIT))) != 3)
    {
        printf("[Opener] Waiting for pool to be clean AND full...\n");
        pthread_cond_wait(&is_full, &mutex);
    }

    // The pool is ready. Open it using Bit 3!
    pool->state |= BIT(OPEN_BIT);
    
    if(pool->state == READY_MASK)
    {
        printf("[Opener] ALL SYSTEMS GO. The pool is now OPENED! Final Mask: 0x%02X\n", pool->state);
    }
    else
    {
        fprintf(stderr, "State mismatch. Expected 0x%02X but got 0x%02X.\n", READY_MASK, pool->state);
    }

    pthread_mutex_unlock(&mutex);

    return NULL;
}

/**
 * @brief Main execution function coordinating the pool lifecycle threads.
 * @return int Exit status.
 */
int main(void)
{
    Pool *pool = NULL;
    init_pool(&pool);
    printf("Pool space allocated at %p\n\n", (void*)pool);
    
    pthread_t cleaner, filler, opener;
    
    // Spawn all workers simultaneously
    pthread_create(&cleaner, NULL, clean, (void *)pool);
    pthread_create(&filler, NULL, fill, (void *)pool);
    // Renamed to open_pool to prevent conflict with standard UNIX open()
    pthread_create(&opener, NULL, open_pool, (void *)pool); 
    
    // Wait for all workers to finish
    pthread_join(cleaner, NULL);
    pthread_join(filler, NULL);
    pthread_join(opener, NULL);

    printf("\nFinal Pool state = %d (0x%02X)\n", pool->state, pool->state);

    free(pool);
    return 0;
}
