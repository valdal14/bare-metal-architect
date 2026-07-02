#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#define VIEWS_COUNT 4

// Concurrency Primitives
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sem_t *items_in_queue;
// File-scoped static array
static const char *const DB_VIEWS[VIEWS_COUNT] = {
    "users_view",
    "orders_view",
    "inventory_view",
    "logs_view"
};

typedef struct Data 
{
    // 17 + 7 paddings = 24 bytes 
    struct Data *next;
    char *view;
} Data;

typedef struct List
{
    struct Data *head;
    struct Data *tail;
} List;

typedef struct Processor 
{
    // 10 + 6 paddings = 16 bytes  
    struct List **list;
    uint8_t capacity;
    uint8_t size;
} Processor;

typedef struct 
{
    struct Processor *processor;
    char *view;
} ProcessingRequest;

/**
 * @brief Hashes a string key into a valid array index.
 * @param key The string to hash.
 * @return uint8_t The calculated index.
 */
uint8_t hash_function(const char *key) {
    int hash = 0;
    
    while (*key != '\0') 
    {
        hash = (hash + *key) % VIEWS_COUNT;
        key++;
    }
    
    return hash;
}

/**
 * @brief The purpose of this function is to  validate if a given  
 * view's name (key) is found in the DB_VIEWS. It does it by 
 * hashing the key and checking whether the view is present.
 * If it is, returns true. if there is a collision it will 
 * perform a O(n) runtime search and scan the entire DB_VIEWS
 * looking for it. If it is found returns true otherwise the 
 * function will return false. 
 *
 * Best  case runtime O(1)
 * Worst case runtime O(n)
 *
 * @param const char key pointer 
 * @return bool
 */
bool verify_view(const char *key)
{
    uint8_t view_idx = hash_function(key);
    const char *view = DB_VIEWS[view_idx];
   
    if(strcmp(key, view) == 0)
        return true;
    else
        for(uint8_t i = 0; i < VIEWS_COUNT; i++)
            if(strcmp(key, DB_VIEWS[i]) == 0) return true;

    return false;
}

/**
 * @brief Inits the Processor
 * @param Processor processor double pointer 
 * @return void
 */
void init_processor(Processor **processor)
{
    Processor *p = (Processor *)calloc(1, sizeof(Processor));
    
    if(p == NULL) 
    {
        fprintf(stderr, "Could not allocate space for the Processor\n");
        exit(EXIT_FAILURE);
    }

    p->list = (List **)calloc(VIEWS_COUNT, sizeof(List *));

    if(p->list == NULL)
    {
        fprintf(stderr, "Could not allocate space for the Processor's List\n");
        exit(EXIT_FAILURE);
    }

    for(uint8_t i = 0; i < VIEWS_COUNT; i++)
    {
        List *new_list = (List *)malloc(sizeof(List));
        new_list->head = NULL;
        new_list->tail = NULL;

        if(new_list == NULL)
        {
            fprintf(stderr, "Could not allocate space for the List\n");
            exit(EXIT_FAILURE);
        }

        p->list[i] = new_list;

    }

    p->capacity = VIEWS_COUNT;
    p->size = 0;

    *processor = p;
}

/**
 * @brief The purpose of this method is to add a new view to the 
 * List of views that needs to be processed. Uses a callback to 
 * evaluate whether the given view's name is in the list of the 
 * supported views. 
 *
 * @param Processor processor pointer
 * @param const char view_name pointer. The view's name 
 * @param bool(*verify)(const char *view_name). The verify callback
 * @return void
 */
void add_view(Processor *processor, const char *view_name, bool(*verify)(const char *view_name))
{
    if(!verify(view_name))
    {
        fprintf(stderr, "ERROR: %s is not a valid view\n", view_name);
        return;
    }
    
    Data *data = (Data *)malloc(sizeof(Data));
    size_t view_length = (strlen(view_name) + 1);
    data->view = (char *)malloc(view_length);
    strncpy(data->view, view_name, view_length);
    data->next = NULL;

    uint8_t list_idx = hash_function(data->view);
    List *list = processor->list[list_idx];

    pthread_mutex_lock(&lock);
    
    if(list->head == NULL)
    {
        list->head = data;
        list->tail = data;
    }
    else
    {
        list->tail->next = data;
        list->tail = data;
    }
    
    processor->size += 1; 
    pthread_mutex_unlock(&lock);

    // SEMAPHORE
    sem_post(items_in_queue);
}

/**
 * @brief Consumes a view from the processor list 
 * @param Processor processor pointer
 * @return void
 */
void consume_any(Processor *processor)
{
    for(uint8_t i = 0; i < VIEWS_COUNT; i++)
    {
        List *list = processor->list[i];
        
        if(list->head != NULL)
        {
            Data *current = list->head;
            
            // Unlink the node
            list->head = current->next;
            if(list->head == NULL) list->tail = NULL;
            
            processor->size -= 1;
            
            // Free the node
            printf("\t[Consumer] Processed and freed view: %s\n", current->view);
            free(current->view);
            free(current);
            
            return;
        }
    }
}

/**
 * @brief Callback executed from a thread that internally calls
 * the add_view method.
 * @param void arg pointer
 * @return void pointer
 */
void *producer_request(void *arg)
{
    ProcessingRequest *request = (ProcessingRequest *)arg;
    add_view(request->processor, request->view, verify_view);
    return NULL;
}

/**
 * @brief Callback executed from a thread that internally calls
 * the consume_any method.
 * @param void arg pointer
 * @return void pointer
 */
void *consumer_process(void *arg)
{
    Processor *processor = (Processor *)arg;
    
    // SEMAPHORE: Wait for a token. If 0, sleep. If > 0, instantly subtract 1 and proceed!
    sem_wait(items_in_queue);
    // MUTEX: We have a token, now safely lock the data structure to take our item.
    pthread_mutex_lock(&lock);
    consume_any(processor);
    pthread_mutex_unlock(&lock);

    return NULL;
}

/**
 * @brief Cleans up the Processor once the processing of the views has 
 * been completed.
 * @param void arg pointer
 * @return void pointer
 */
void *end_process(void *arg)
{
    Processor *processor = (Processor *)arg;
    
    // Because main() joins all threads, No waiting needed!
    for(uint8_t i = 0; i < VIEWS_COUNT; i++)
    {
        free(processor->list[i]);
    }
    
    free(processor->list);
    free(processor);

    processor = NULL;
    return NULL;
}

int main(void)
{
    // Initialize the Semaphore to 0 tokens
    sem_unlink("/items_queue"); 
    items_in_queue = sem_open("/items_queue", O_CREAT, 0644, 0);
    
    if (items_in_queue == SEM_FAILED) 
    {
        fprintf(stderr, "Failed to initialize semaphore\n");
        exit(EXIT_FAILURE);
    }
    
    Processor *processor = NULL;
    init_processor(&processor);

    ProcessingRequest requests[VIEWS_COUNT];
    requests[0] = (ProcessingRequest){processor, "logs_view"};
    requests[1] = (ProcessingRequest){processor, "inventory_view"}; 
    requests[2] = (ProcessingRequest){processor, "orders_view"};
    requests[3] = (ProcessingRequest){processor, "users_view"};

    // Thread Arrays
    pthread_t add_threads[VIEWS_COUNT];
    pthread_t execute_threads[VIEWS_COUNT];

    for(uint8_t i = 0; i < VIEWS_COUNT; i++)
    {
        printf("[Main] Spawning Producer for %s\n", requests[i].view); 
        sleep(1);
        pthread_create(&add_threads[i], NULL, producer_request, (void *)&requests[i]);
        
        printf("[Main] Spawning generic Consumer worker...\n"); 
        sleep(2);
        pthread_create(&execute_threads[i], NULL, consumer_process, (void *)processor);
    }
    
    // Join all threads safely
    for(uint8_t i = 0; i < VIEWS_COUNT; i++)
    {
        pthread_join(add_threads[i], NULL);
        pthread_join(execute_threads[i], NULL);
    }
    
    printf("\nAll processing complete. Cleaning up Data Structures...\n");
    
    pthread_t clean;
    pthread_create(&clean, NULL, end_process, (void *)processor);
    pthread_join(clean, NULL);

    // Destroy the semaphore to return memory to the OS
    sem_close(items_in_queue);
    sem_unlink("/items_queue");
    return 0;
}
