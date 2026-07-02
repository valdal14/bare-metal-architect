#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define VIEWS_COUNT 4

// Concurrency Primitives
pthread_mutex_t views_lock = PTHREAD_MUTEX_INITIALIZER;

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

typedef struct 
{
    // 10 + 6 paddings = 16 bytes  
    struct List **list;
    uint8_t capacity;
    uint8_t size;
} Producer;

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
 * @brief Inits the Producer 
 * @param Producer producer double pointer 
 * @return void
 */
void init_producer(Producer **producer)
{
    Producer *p = (Producer *)calloc(1, sizeof(Producer *));
    
    if(p == NULL) 
    {
        fprintf(stderr, "Could not allocate space for the Producer\n");
        exit(EXIT_FAILURE);
    }

    p->list = (List **)calloc(VIEWS_COUNT, sizeof(List *));

    if(p->list == NULL)
    {
        fprintf(stderr, "Could not allocate space for the Producer's List\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < VIEWS_COUNT; i++)
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

    *producer = p;
}

/**
 * @brief The purpose of this method is to add a new view to the 
 * List of views that needs to be processed. Uses a callback to 
 * evaluate whether the given view's name is in the list of the 
 * supported views. 
 *
 * @param Producer producer pointer
 * @param const char view_name pointer. The view's name 
 * @param bool(*verify)(const char *view_name). The verify callback
 * @return void
 */
void add_view(Producer *producer, const char *view_name, bool(*verify)(const char *view_name))
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

    if(producer->list[list_idx]->head == NULL)
    {
        producer->list[list_idx]->head = data;
        producer->list[list_idx]->tail = data;
    }
    else
    {
        List *list = producer->list[list_idx];
        list->tail->next = data;
        list->tail = data;
    }
}

int main(void)
{
    Producer *producer = NULL;
    init_producer(&producer);
    printf("Producer allocated at address %p\n", producer);
    add_view(producer, "logs_view", verify_view); 
    add_view(producer, "inventory_view", verify_view); 
    add_view(producer, "orders_view", verify_view); 
    add_view(producer, "users_view", verify_view); 
    
    printf("%s\n", producer->list[0]->head->view);
    printf("%s\n", producer->list[0]->tail->view);

    
    return 0;
}
