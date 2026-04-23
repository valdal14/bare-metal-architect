#include <stdio.h>
#include <stdlib.h>

#define LIST 0
#define NODE 1

struct Node 
{
    int value;
    struct Node *next;
    struct Node *prev;
};

struct DoublyLinkedList 
{
    struct Node *head;
    struct Node *tail;
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
            if((struct DoublyLinkedList *)p == NULL)
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
 * @brief Factory method that create an instance of the DLL
 * on the heap
 * @return struct DoublyLinkedList Pointer
 */
struct DoublyLinkedList* dll_create(void)
{
    struct DoublyLinkedList *dll = (struct DoublyLinkedList *)calloc(1, sizeof(struct DoublyLinkedList *));
    check_alloc(dll, LIST);
    
    dll->head = NULL;
    dll->tail = NULL;
    dll->size = 0;

    return dll;
}

/**
 * @brief Append a new integer value to the DoublyLinkedList
 * @param dll struct DoublyLinkedList Pointer
 * @param value The value that we want to store
 * @return void
 */
void dll_append(struct DoublyLinkedList *dll, int value)
{
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    check_alloc(node, NODE);
    node->value = value;
    node->next = NULL;

    if(dll->head == NULL)
    {
        node->prev = NULL;
        dll->head = node;
        dll->tail = node;
    }
    else 
    {
        // Point new node back to the old tail
        node->prev = dll->tail;
        
        // Point old tail forward to the new node (The missing link!)
        dll->tail->next = node;
        
        // Upgrade the list's tail pointer
        dll->tail = node;
    }
    
    dll->size += 1;
}

/**
 * @brief Print out the value of the DLL starting from the tail
 * @param struct DoublyLinkedList Pointer
 * @return void
 */
void dll_print_reverse(struct DoublyLinkedList *dll)
{
    struct Node *current = dll->tail;
    int counter = dll->size;

    while(current->prev != NULL)
    { 
        printf("At index %d we stored the value %d\n", (counter - 1), current->value);
        current = current->prev;
        counter -= 1;
    }

    printf("At index %d we stored the value %d\n", (counter - 1), current->value);
}

/**
 * @brief Safely removes all allocated nodes and the list wrapper from the heap.
 * @param struct DoublyLinkedList Pointer
 * @return void
 */
void dll_free(struct DoublyLinkedList *dll)
{
    struct Node *current = dll->tail;

    while(current->prev != NULL)
    {
        current = current->prev;
        free(current->next);
    }
    
    free(current);
    free(dll);
}

int main(void)
{
    struct DoublyLinkedList *dll = dll_create();
    
    dll_append(dll, 14);
    dll_append(dll, 7);
    dll_append(dll, 13);
    dll_append(dll, 19);
    dll_append(dll, 4);
    dll_append(dll, 114);

    dll_print_reverse(dll);

    dll_free(dll);
    
    return 0;
}



