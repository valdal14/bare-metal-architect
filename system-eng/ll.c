#include <stdio.h>
#include <stdlib.h>

typedef struct Node 
{
    int data;
    struct Node *next;
} Node;

/**
 * @brief Add a new Node to the list
 * @param Node head_ref double pointer
 * @param int new_data
 * @return void
 */
void add_node(Node **head_ref, int new_data)
{
    Node *node = (Node *)malloc(sizeof(Node));
    if(node == NULL) exit(1);
    node->data = new_data;
    node->next = NULL;

    if(*head_ref == NULL)
    {
        *head_ref = node;
    }
    else
    {
        Node *current = *head_ref;
        
        while(current->next != NULL)
        {
            current = current->next;
        }

        current->next = node;
    }
}

/**
 * @brief Prints the nodes
 * @oaram Node head pointer
 * @return void
 */
void print_list(Node *head)
{
    Node *current = head;
    int count = 0;

    while(current != NULL)
    {
        printf("[%d] data = %d\n", count, current->data);
        count++;
        current = current->next;
    }
}

/**
 * @brief Deallocate Nodes from the heap 
 * @oaram Node head pointer
 * @return void
 */
void free_nodes(Node **head_ref)
{
    Node *current = *head_ref;
    Node *next_node = NULL;

    while(current != NULL)
    {
        next_node = current->next;
        free(current);
        current = next_node;
    }
}

int main(void)
{
    Node *head = NULL;
    add_node(&head, 10);
    add_node(&head, 20);
    add_node(&head, 30);
    
    print_list(head);

    free_nodes(&head);
    
    return 0;
}
