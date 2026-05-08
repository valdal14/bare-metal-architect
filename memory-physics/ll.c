#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_SIZE 32

struct List
{
    char *data;
    struct List *next;
};

struct List *init(void)
{
    struct List *list = (struct List *)calloc(1, sizeof(struct List));
    if(list == NULL) exit(1);
    
    list->data = NULL;
    list->next = NULL;
    return list;
}

void add_node(struct List *list, char *value)
{
    if(list->data == NULL)
    {
        list->data = (char *)malloc(DATA_SIZE);
        strncpy(list->data, value, DATA_SIZE);
        list->data[DATA_SIZE - 1] = '\0';
        list->next = NULL;
    }
    else
    {
        struct List *new_node = (struct List *)malloc(sizeof(struct List));
        if(new_node == NULL) exit(1);
        new_node->data = (char *)malloc(DATA_SIZE);
        strncpy(new_node->data, value, DATA_SIZE);
        new_node->data[DATA_SIZE - 1] = '\0';
        new_node->next = NULL;

        struct List *current = list;
        
        while(current->next != NULL)
        {
            current = current->next;
        }
       
        current->next = new_node;
    }
}

void remove_last(struct List **list)
{
    if(*list == NULL)
    {
        printf("Nothing to remove from an empty list\n");
        exit(1);
    }
    else
    {
        struct List *current = *list;
        struct List *previous = NULL;
        int has_one_node = 0;

        while(current->next != NULL)
        {
            previous = current;  
            current = current->next; 
            has_one_node++;
        }
        
        if(has_one_node == 0)
        {
            *list = NULL;
            free(current);
        }
        else
        {
            previous->next = NULL; 
            free(current);
        }
    }
}

void print(struct List *list)
{
    int counter = 0;
    struct List *current = list;

    while(current != NULL)
    {
        printf("Node[%d] stores value = %s\n", counter, current->data);
        current = current->next;
        counter++;
    }

    printf("-------------------------\n");
}

void free_list(struct List *list)
{
    struct List *current = list;
    struct List *next_node = NULL;

    while(current != NULL)
    {
        next_node = current->next;
        free(current->data);
        free(current);
        current = next_node;
    }
}

int main(void)
{
    struct List *list = init();
    add_node(list, "Hello");
    add_node(list, "Ciao");
    add_node(list, "Hola");
    add_node(list, "Beer");
    print(list);
    remove_last(&list);
    print(list);
    free_list(list);
    return 0;
}
