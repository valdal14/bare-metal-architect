#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUEUE_MSN_SIZE 254 
#define PRINTER_CAPACITY 5

struct Queue
{
    char messages[PRINTER_CAPACITY][QUEUE_MSN_SIZE];
};

struct Printer
{
    struct Queue *queue;
    int size;
    int capacity;
};

/**
 * @brief Init Factory method to instanicate the Printer
 * @return struct Printer pointer
 */
struct Printer *init(void)
{
    struct Printer *p = (struct Printer *)calloc(1, sizeof(struct Printer));    
    if(p == NULL) exit(1);

    // allocate the Queue associated with the Printer
    p->queue = (struct Queue *)calloc(1, sizeof(struct Queue));
    if(p->queue == NULL) exit(1);

    p->size = 0;
    p->capacity = PRINTER_CAPACITY;
    return p;
}

/*
 * @brief Handle the message queue for upcoming messages
 * and dequeue automatically if full capacity is reached 
 * @param struct Printer Pointer
 * @param char msn The message we want to print
 * @return void
 */
void enqueue(struct Printer *p, char *msn)
{
    if(p->size == p->capacity)
    {
        printf("Perform dequeue operation before pushing a new message in the queue\n");
        exit(1);
    }

    strncpy(p->queue->messages[p->size], msn, strlen(msn) + 1);
    
    // Ensure null-termination just in case the string was too long
    p->queue->messages[p->size][QUEUE_MSN_SIZE - 1] = '\0';
    p->size = p->size + 1;
}

/*
 * @brief Dequeue the message from the Printer's Queue
 * @param struct Printer Pointer
 * @return char Pointer
 */
char *dequeue(struct Printer *p)
{
    if(p->size == 0)
    {
        fprintf(stderr, "Cannot pop from empty queue\n");
        exit(1);
    }
    
    // get the size of the message at index 0 + the null terminator \0
    int msn_size = strlen(p->queue->messages[0]) + 1;
    // allocate space to returned it
    char *dequeued_msn = (char *)malloc(msn_size);
    if(dequeued_msn == NULL) exit(1);
    // Copy the string at index 0
    strncpy(dequeued_msn, p->queue->messages[0], msn_size);

    // Shift all remaining messages forward by one position.
    for(int i = 0; i < p->size - 1; i++)
    {
        strncpy(p->queue->messages[i], p->queue->messages[i + 1], QUEUE_MSN_SIZE);
    }
    
    p->size = p->size - 1;
    return dequeued_msn;
}

/**
 * @brief Display the message to the user
 * @param struct Printer Pointer
 * @return void
 */
void print_queue(struct Printer *p)
{
    if(p->size == 0)
    {
        fprintf(stderr, "Cannot print from an emtpy queue\n");
        exit(1);
    }

    printf("-------------- Print Queued Messages --------------\n");

    for(int i = 0; i < p->size; i++)
    {
        printf("MSN%d: %s\n", i, p->queue->messages[i]);
    }
    printf("---------------------------------------------------\n");
}

/**
 * @brief Memory deallocation
 * @param struct Printer pointer
 * @return void
 */
void free_ram(struct Printer *p)
{
    free(p->queue);
    free(p);
}

int main(void)
{
    struct Printer *p = init();
    
    enqueue(p, "Hello World!");
    enqueue(p, "Ciao Mondo!");
    enqueue(p, "Hola Mundo!");
    
    char *dq_msn = dequeue(p);
    printf("Dequeued msn: %s\n", dq_msn);
 
    print_queue(p);
    
    free(dq_msn);
    free_ram(p);
    return 0;
}
