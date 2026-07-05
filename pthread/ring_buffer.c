#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFFER_SIZE 10

// Concurrency Global Primitives
sem_t *empty_slots; // Tracks how much free space is left in the array
sem_t *filled_slots; // Tracks how many items are currently in the array waiting to be consumed
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct
{
    // 12 bytes + 4 padding struct
    uint8_t buffer[BUFFER_SIZE];
    uint8_t write_index;
    uint8_t read_index;
} RingBuffer;

/**
 * @brief Initialize the Semaphore to 0 tokens
 * @param const char identifier pointer
 * @param uint8_t init_size
 * @return sem_t pointer
 */
sem_t *init_semaphore(const char *identifier, uint8_t init_size)
{
    sem_unlink(identifier);
    // Added O_EXCL to strictly forbid reusing an old, deadlocked semaphore
    sem_t *s = sem_open(identifier, O_CREAT | O_EXCL, 0644, init_size);
    
    if(s == SEM_FAILED) {
        fprintf(stderr, "Fatal: OS refused to open %s. A ghost process might be holding it!\n", identifier);
        exit(EXIT_FAILURE);
    }
    return s;
}

/**
 * @brief Destroys and unlink the Semaphore
 * @param sem_t semaphore pointer
 * @return void
 */
void destroy_semaphore(sem_t *semaphore, const char *identifier)
{
    sem_close(semaphore);
    sem_unlink(identifier);
}

// --------------------------------------------------------------

/**
 * @brief Inits the RingBuffer struct 
 * @param RingBuffer double pointer
 * @return void
 */
void init(RingBuffer **ring_buffer)
{
   RingBuffer *rb = (RingBuffer *)calloc(1, sizeof(RingBuffer));
   
   if(rb == NULL)
   {
       fprintf(stderr, "Could not allocate space for the RingBuffer\n");
       exit(EXIT_FAILURE);
   }

   rb->write_index = 0;
   rb->read_index = 0;

   *ring_buffer = rb;
}

// --------------------------------------------------------------

/**
 * @brief Pthread's Callback used to add values to the RingBuffer 
 * buffer.
 * @param void arg pointer
 * @return void pointer
 */
void *add_value(void *arg)
{
    RingBuffer *rb = (RingBuffer *)arg;
    
    // Each producer writes 10 items
    for(uint8_t i = 0; i < 10; i++) 
    {
        // wait
        sem_wait(empty_slots);
        // lock
        pthread_mutex_lock(&lock); 
        rb->buffer[rb->write_index] = i;
        printf("[Producer] Wrote %d at index %d\n", i, rb->write_index);
        rb->write_index = (rb->write_index + 1) % BUFFER_SIZE;
        // Unlock the array
        pthread_mutex_unlock(&lock); 
        // Post 1 item is ready
        sem_post(filled_slots);        
        // Sleep for 0.1s to simulate work
        usleep(100000);                
    }

    return rb;
}

/**
 * @brief Pthread's Callback used to get values from the RingBuffer 
 * buffer.
 * @param void arg pointer
 * @return void pointer
 */
void *get_value(void *arg)
{
    RingBuffer *rb = (RingBuffer *)arg;
    
    for(uint8_t i = 0; i < 10; i++) 
    {
        // Wait for 1 available item
        sem_wait(filled_slots);        
        // Lock the array
        pthread_mutex_lock(&lock);     
        uint8_t val = rb->buffer[rb->read_index];
        printf("\t[Consumer] Read %d from index %d\n", val, rb->read_index);
        rb->read_index = (rb->read_index + 1) % BUFFER_SIZE;
        // Unlock the array
        pthread_mutex_unlock(&lock);   
        // Post 1 slot is now free
        sem_post(empty_slots);
        // Sleep for 0.15s
        usleep(150000); 
    }
    return rb;
}

/**
 * @brief Reads the final values stored in the RingBuffer
 * @param RingBuffer rb pointer
 * @return void
 */
void read_data(RingBuffer *rb)
{
    for(uint8_t i = 0; i < BUFFER_SIZE; i++)
    {
        printf("Buffer[%d] = %d\n", i, rb->buffer[i]);
        sleep(1); 
    } 
}

// --------------------------------------------------------------

int main(void)
{
    // Init the RingBuffer
    RingBuffer *rb = NULL;
    init(&rb);
    printf("RingBuffer allocated at address %p\n", rb);
    // Init the Semaphores 
    empty_slots = init_semaphore("/empty_slots", BUFFER_SIZE);
    filled_slots = init_semaphore("/filled_slots", 0);
    // Init the POSIX Thread 
    pthread_t prod1, prod2, cons; 

    pthread_create(&prod1, NULL, add_value, (void *)rb);
    pthread_create(&prod2, NULL, add_value, (void *)rb);
    pthread_create(&cons, NULL, get_value, (void *)rb);
    
    // Join POSIX Thread
    RingBuffer *consumed_rb = NULL;
    pthread_join(prod1, NULL);
    pthread_join(prod2, NULL);
    pthread_join(cons, (void **)&consumed_rb);

    /**
     * Read the value stored in the RingBuffer
     * returned by the get_value callback when
     * the thread joins the main thread
     */
    read_data(consumed_rb);

    // Destroy the Semaphores
    destroy_semaphore(empty_slots, "/empty_slots");
    destroy_semaphore(filled_slots, "/filled_slots");
    return 0;
}
