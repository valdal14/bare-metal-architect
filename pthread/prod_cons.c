#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define VIEWS_COUNT 4

// Concurrency Primitives
pthread_cond_t is_added = PTHREAD_COND_INITIALIZER;
pthread_cond_t is_processed = PTHREAD_COND_INITIALIZER;
pthread_cond_t is_empty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
bool wait_for_add = true;
bool wait_for_process = true;
/* File-scoped static array. Kept private to this file. */
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
    Processor *p = (Processor *)calloc(1, sizeof(Processor *));
    
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
    
    uint8_t is_valid = verify(view_name);
    
    if(is_valid == false)
    {
        fprintf(stderr, "ERROR: %s is not a valid view\n", view_name);
        fprintf(stderr, "%s will not be processed\n", view_name);
        return;
    }
    
    // allocate space for the data 
    Data *data = (Data *)malloc(sizeof(Data));
    
    if(data == NULL)
    {
        fprintf(stderr, "Could not allocate space for the view %s\n", view_name);
        fprintf(stderr, "%s will not be processed\n", view_name);
        return;
    }

    // copy the verified view_name
    size_t view_length = (strlen(view_name) + 1);
    data->view = (char *)malloc(sizeof(char) * view_length);
    strncpy(data->view, view_name, view_length);
    data->view[view_length] = '\0';
    
    data->next = NULL;
    uint8_t list_idx = hash_function(data->view);

    if(processor->list[list_idx]->head == NULL)
    {
        processor->list[list_idx]->head = data;
        processor->list[list_idx]->tail = data;
    }
    else
    {

        while(wait_for_process) pthread_cond_wait(&is_processed, &lock);
        
        List *list = processor->list[list_idx];
        list->tail->next = data;
        list->tail = data;
    }

    processor->size += 1; 
    wait_for_add = false;
    pthread_cond_signal(&is_added);
}

/**
 * @brief Consumes a view from the processor list 
 * @param Processor processor pointer
 * @param const char view_name pointer
 * @return void
 */
void consume_view(Processor *processor, const char *view_name)
{
    List **list = processor->list;
    uint8_t index = hash_function(view_name);
    
    if(list[index] == NULL)
    {
        fprintf(stderr, "Could not find any view named %s in the processing list\n", view_name);
        return;
    }

    Data *head = list[index]->head;
    Data *tail = list[index]->tail;

    if((strcmp(head->view, view_name) == 0) || (strcmp(tail->view, view_name) == 0))
    {
        processor->size -= 1;
        if(processor->size == 0) pthread_cond_signal(&is_empty);
    }
    else
    {
        while(head == NULL)
        {
            if(strcmp(head->view, view_name) == 0)
            {
                processor->size -= 1;
                if(processor->size == 0) pthread_cond_signal(&is_empty);
                return;
            }

            head = head->next;
        }
    }

    wait_for_process = false;
    pthread_cond_signal(&is_processed);
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
    if(processor == NULL) return NULL;

    pthread_mutex_lock(&lock);
   
    // wait to clean up until all views have been processed
    while(processor->size != 0) pthread_cond_wait(&is_empty, &lock);
     
    for(uint8_t i = 0; i < VIEWS_COUNT; i++)
    {
        Data *current = processor->list[i]->head;
        Data *next_node = NULL;

        while(current != NULL)
        {
            next_node = current->next;
            free(current->view);
            free(current);
            current = next_node;
        }
    }

    free(processor->list);
    free(processor);
    processor = NULL;

    pthread_mutex_unlock(&lock);

    return NULL;
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
    if(request == NULL) return NULL;
    pthread_mutex_lock(&lock);
    // internally calls the add_view to add the new view to the processor list 
    add_view(request->processor, request->view, verify_view);
    pthread_mutex_unlock(&lock);

    return NULL;
}

void *consumer_process(void *arg)
{
    ProcessingRequest *request = (ProcessingRequest *)arg;
    if(request == NULL) return NULL;
    pthread_mutex_lock(&lock);
    
    while(wait_for_add)
    {
        pthread_cond_wait(&is_added, &lock);
    }

    consume_view(request->processor, request->view);
    if(request->processor->size == 0) pthread_cond_signal(&is_empty);
    pthread_mutex_unlock(&lock);

    return NULL;
}

int main(void)
{
    pthread_t clean, add, execute;
    Processor *processor = NULL;
    init_processor(&processor);

    // ProcessingRequest (Simulating upcoming requests)
    ProcessingRequest requests[VIEWS_COUNT];
    ProcessingRequest r1, r2, r3, r4;
    r1.processor = processor;
    r1.view = "logs_view";
    r2.processor = processor;
    r2.view = "inventory_view"; 
    r3.processor = processor;
    r3.view = "orders_view";
    r4.processor = processor;
    r4.view = "users_view";
    requests[0] = r1;
    requests[1] = r2;
    requests[2] = r3;
    requests[3] = r4;

    for(uint8_t i = 0; i < VIEWS_COUNT; i++)
    {

        printf("Adding the new view named %s\n", requests[i].view);
        sleep(1);
        pthread_create(&add, NULL, producer_request, (void *)&requests[i]);
        printf("%s added to the list of the processing view\n", requests[i].view);
        printf("--------------------------------------------------------------\n");
        printf("Processing the view %s, please wait..\n", requests[i].view); 
        sleep(2);
        pthread_create(&execute, NULL, consumer_process, (void *)&requests[i]);
        printf("%s processing completed\n", requests[i].view);
        printf("--------------------------------------------------------------\n");
    }
    
    printf("\nProcessor Completed. The Processor will be cleaned\n");
    pthread_create(&clean, NULL, end_process, (void *)processor);

    pthread_join(add, NULL);
    pthread_join(execute, NULL);
    pthread_join(clean, NULL);

    return 0;
}
