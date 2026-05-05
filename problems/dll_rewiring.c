#include <stdio.h>
#include <stdlib.h>

struct DNode 
{
    int data;
    struct DNode *next;
    struct DNode *prev;
};


/**
 * @brief Init the DLL with data
 * @return void
 */
struct DNode *init(void)
{
    struct DNode *head = (struct DNode *)calloc(1, sizeof(struct DNode));
    if(head == NULL) exit(1);
    head->data = 1;
    head->next = NULL;
    head->prev = NULL;

    
    struct DNode *nd1 = (struct DNode *)calloc(1, sizeof(struct DNode));
    if(nd1 == NULL) exit(1);
    nd1->data = 2;
    head->next = nd1;
    nd1->prev = head;
    nd1->next = NULL;

    struct DNode *nd2 = (struct DNode *)calloc(1, sizeof(struct DNode));
    if(nd2 == NULL) exit(1);
    nd2->data = 3;
    nd1->next = nd2;
    nd2->prev = nd1;
    nd2->next = NULL;

    return head;
}

void print(struct DNode *head)
{
    struct DNode *current = head;
    
    while(current != NULL)
    {
        printf("C %d\n", current->data);
        current = current->next;
    }
}

/**
 * @brief Reverses a Doubly Linked List in-place by rewiring the next and prev pointers.
 *        Updates the original head pointer to point to the new head of the reversed list.
 * @param head_ref Double pointer to the head of the doubly linked list.
 * @return void
 */
void reverse_dll(struct DNode **head_ref)
{
    struct DNode *current = *head_ref;
    struct DNode *temp = NULL;
    
    struct DNode *last_visited = NULL; 

    while (current != NULL) 
    {
        temp = current->next;
        
        // The Swap
        current->next = current->prev;
        current->prev = temp;
        
        // Save the current last visited before reassigning the current
        last_visited = current; 
        
        // 4. Move forward
        current = temp;
    }

    // 'current' is NULL. 
    // 'last_visited' is holding the very last node, and it is the new head
    if (last_visited != NULL) 
    {
        *head_ref = last_visited;
    }
}

int main(void)
{
    struct DNode *dll = init();
    reverse_dll(&dll);
    print(dll);
    return 0;
}
