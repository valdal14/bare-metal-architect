#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#define VALID_PACKAGE_MASK 0x8B
#define BIT(x) (1 << (x))
#define ACK_IDX 0 
#define SEC_IDX 1 
#define VER_IDX 3
#define VAL_IDX 7
#define PACKAGE_ITER 8

/**
 * decoding time: 0,516 
 * L = λW
 * λ = 255
 * W = 0.516 
 *
 * L = 131.58 
 * Queue Size = 132
 */
#define QUEUE_SIZE 132 
#define PACKAGE_X_SEC 255 

// Concurrency Primitives 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t can_read = PTHREAD_COND_INITIALIZER;

typedef struct Queue
{
    uint8_t *packets;
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} Queue;

typedef struct Package 
{
    /**
     * bit 0 -> ack 
     * bit 2 -> sec 
     * bit 4 -> ver
     * bit 8 -> val 
     *
     * valid package value: 1000 1011 = 139
     */
    uint8_t packet;
} Package;

typedef struct
{
    struct Package value;
    struct Queue *queue;
} Encoding;

/**
 * @brief Inits the Queue
 * @param Queue queue double pointer
 * @return void
 */
void init_queue(Queue **queue)
{
    Queue *bucket = (Queue *)calloc(1, sizeof(Queue));
    
    if(bucket == NULL)
    {
        fprintf(stderr, "Could not allocate space for the queue\n");
        exit(EXIT_FAILURE);
    }

    bucket->packets = (uint8_t *)calloc(QUEUE_SIZE, sizeof(uint8_t));

    if(bucket->packets == NULL)
    {
        fprintf(stderr, "Could not allocate space for the packets space\n");
        exit(EXIT_FAILURE);
    }
    
    bucket->head = 0;
    bucket->tail = 0;
    bucket->count = 0;

    *queue = bucket;
}

/**
 * @brief POSIX Thread callback responsible of encoding the 
 * upcoming package based on the supported value 
 * @param void arg pointer
 * @return void pointer
 */
void *encode_package(void *arg)
{
    Encoding *package = (Encoding *)arg;
    if(package == NULL) return NULL;

    pthread_mutex_lock(&lock);

    uint8_t incoming = package->value.packet;
    printf("Incoming package value: %d\n", incoming);
    
    while(package->queue->count == QUEUE_SIZE)
    {
        fprintf(stderr, "Queue is full, wait for the system to start decoding...\n");
        pthread_cond_wait(&not_full, &lock);
    }


    for(uint8_t i = 0; i < PACKAGE_ITER; i++)
        if((i & BIT(i)) == 0) package->value.packet &= ~BIT(i);
    
    package->value.packet |= BIT(ACK_IDX);
    package->value.packet |= BIT(SEC_IDX);
    package->value.packet |= BIT(VER_IDX);
    package->value.packet |= BIT(VAL_IDX);

    if((package->value.packet & VALID_PACKAGE_MASK) != 0)
    {
        package->queue->tail = (package->queue->tail + 1) % QUEUE_SIZE;
        package->queue->packets[package->queue->tail] = package->value.packet;
        package->queue->count++;
    }
    
    pthread_mutex_unlock(&lock);

    pthread_cond_signal(&not_empty);
    
    return NULL;
}

/**
 * @brief Callback of the POSIX Thread used to read/decode 
 * the packges stored in the queue. 
 * @param void arg
 * @return void pointer
 */
void *decode_package(void *arg)
{
    Encoding *package = (Encoding *)arg;
    if(package == NULL) return NULL;
    
    pthread_mutex_lock(&lock);
    
    while(package->queue->count == 0)
    {
        pthread_cond_wait(&not_empty, &lock);
    }
    
    package->queue->head = (package->queue->head + 1) % QUEUE_SIZE;
    uint8_t val = package->queue->packets[package->queue->head];
    package->queue->count--;
    
    printf("Deconding package = %d\n", val);
    
    pthread_mutex_unlock(&lock);
    pthread_cond_signal(&not_full);
   
    return NULL;
}

/**
 * @brief Cleans up the allocated memory
 * @param Queue queue pointer
 * @return void
 */
void clean(Queue *queue)
{
    free(queue->packets);
    free(queue);
}

int main(void)
{
    Queue *queue = NULL; 
    init_queue(&queue);
    pthread_t producer[PACKAGE_X_SEC];
    pthread_t consumer[PACKAGE_X_SEC];
    Encoding packages_array[PACKAGE_X_SEC];

    for(uint8_t i = 0; i < PACKAGE_X_SEC; i++)
    {
        packages_array[i].queue = queue;
        packages_array[i].value.packet = i;
        pthread_create(&producer[i], NULL, encode_package, (void *)&packages_array[i]);
    }

    for(uint8_t i = 0; i < PACKAGE_X_SEC; i++)
    {
        pthread_create(&consumer[i], NULL, decode_package, (void *)&packages_array[i]);
    }
    
    for(uint8_t i = 0; i < PACKAGE_X_SEC; i++)
    { 
        pthread_join(producer[i], NULL); 
        pthread_join(consumer[i], NULL); 
    }
    
    clean(queue);

    return 0;
}
