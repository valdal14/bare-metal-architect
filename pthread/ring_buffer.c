#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

// Concurrency Global Primitives
sem_t *empty_slots; // Tracks how much free space is left in the array
sem_t *filled_slots; // Tracks how many items are currently in the array waiting to be consumed
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct
{
    // 16 bytes + 0 padding struct
    uint8_t buffer[14];
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
    return sem_open(identifier, O_CREAT, 0644, init_size);
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

int main(void)
{
    // Init the RingBuffer
    RingBuffer *rb = NULL;
    init(&rb);
    printf("RingBuffer allocated at address %p\n", rb);
    // init the Semaphores 
    empty_slots = init_semaphore("/empty_slots", 5);
    filled_slots = init_semaphore("/filled_slots", 0);

    destroy_semaphore(empty_slots, "/filled_slots");
    destroy_semaphore(filled_slots, "/empty_slots");
    return 0;
}
