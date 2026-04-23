#include <stdio.h>
#include <stdlib.h>

#define LIST 0
#define NODE 1

struct Node 
{
    int value;
    struct Node *next;
};

struct LinkedList
{
    struct Node *head;
    int size;
};

/**
 * @brief Check the allocation of the given pointer
 * @param void p Pointer
 * @param int type Either LIST(0) or NODE(1)
 * @return void
 */
void check_alloc(void *p, int type)
{
    int error = 0;

    switch(type)
    {
        case 0:
            if((struct LinkedList *)p == NULL)
            {
                fprintf(stderr, "Cannot allocate memory for the List\n");
                error = 1;
            }
            break;
        case 1:
            if((struct Node *)p == NULL)
            {
                fprintf(stderr, "Cannot allocate memory for the Node\n");
                error = 1;
            }
            break;
        default:
            fprintf(stderr, "Unsupported type\n");
            break;
    }

    if(error == 1) exit(1);
}

/**
 * @brief Factory method that create an instance of the LL
 * on the heap
 * @return struct LinkedList Pointer
 */
struct LinkedList* list_create(void)
{
    struct LinkedList *ll = (struct LinkedList *)calloc(1, sizeof(struct LinkedList));
    check_alloc(ll, LIST);
    ll->head = NULL;
    ll->size = 0;
    return ll;
}

/**
 * @brief Append a new integer value to the LinkedList
 * @param struct LinkedList Pointer
 * @param int value The value that we want to store
 * @return void
 */
void list_append(struct LinkedList *list, int value)
{
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    check_alloc(node, NODE);
    // add the new value to the just allocated node
    node->value = value;
    
    if(list->head == NULL)
    { 
        node->next = NULL;
        list->head = node;
        list->size = (list->size + 1);
        printf("Added root node with value: %d\n", value);
    }
    else
    {
        struct Node *current = list->head;
        
        while(current->next != NULL)
        {
            current = current->next;
        }
         
        node->next = NULL;
        current->next = node;
        list->size = (list->size + 1);
        
        printf("Added child node with value: %d\n", value); 
    }

}

/**
 * @brief Getting a node value by index
 * @param struct LinkedList Pointer
 * @param int index The index stored in the Node
 * @return int The value stored at a given index
 */
int list_get(struct LinkedList *list, int index)
{
    if(index < 0 || index >= list->size)
    {
        fprintf(stderr, "Index OutOfBound Error\n");
        exit(1);
    }

    struct Node *current = list->head;
    int counter = 0;

    while(current->next != NULL)
    {
        if(index == counter)
        {
            return current->value;
        }

        current = current->next;
        counter += 1;
    }

    return current->value;
}

/**
 * @brief Safely removes all allocated nodes and the list wrapper from the heap.
 * @param list struct LinkedList Pointer
 * @return void
 */
void list_free(struct LinkedList *list)
{
    printf("\nMemory Cleanup Process Called ---\n");
    
    struct Node *current = list->head;
    struct Node *next_node = NULL;

    while (current != NULL)
    {
        printf("- Freeing node value = %d\n", current->value);
     
        next_node = current->next;
        free(current);
		
        current = next_node;
    }

    printf("- Freeing LinkedList wrapper\n");
    free(list);
    printf("----------------------------------\n");
}

int main(void)
{
    struct LinkedList *list = list_create();
    list_append(list, 14);
    list_append(list, 7);
    list_append(list, 4);
    list_append(list, 18);
    list_append(list, 114);

    int index = 3;
    int value = list_get(list, index);
    printf("\nAt index %d I found the value = %d\n", index, value);
    
    list_free(list);
    
    return 0;
}
