#include <stdio.h>
#include <stdlib.h>

struct Node
{
    int id;
    int payload_size;
    struct Node *next;
};

void check_node_allocation(struct Node *node)
{
    if(node == NULL)
    {
        fprintf(stderr, "Cannot allocate memory for the new node\n");
        exit(1);
    }
}

struct Node *create_new_node()
{
    struct Node *node = (struct Node*)malloc(sizeof(struct Node));
    check_node_allocation(node);
    return node;
}

void assign_values(struct Node *node, int id, int size, struct Node *next_node)
{
    node->id = id;
    node->payload_size = size;
    node->next = next_node;
}

void append(struct Node **head, int id, int size)
{
    if(*head == NULL)
    {
        struct Node *node = create_new_node();
        assign_values(node, id, size, NULL);
        *head = node;
    }
    else
    {

        struct Node *current = *head;
        check_node_allocation(current); 

        while(current->next != NULL)
        {
            current = current->next; 
        }
        
        struct Node *new_node = create_new_node();
        assign_values(new_node, id, size, NULL);
        // old tail points to the new node
        current->next = new_node;
    }

}

void print_list(struct Node *head)
{
    struct Node *current = head;
    check_node_allocation(current);

    while(current != NULL)
    {
        printf("Node id = %d - Node size = %d\n", current->id, current->payload_size);
        current = current->next;
    }
}

void free_list(struct Node **head) {
    struct Node *current = *head;
    struct Node *next_node;

    while (current != NULL) {
        next_node = current->next; 
        free(current);              
        current = next_node;       
    }

    *head = NULL; 
}

int main(void)
{
    struct Node *head = NULL;
    append(&head, 111, 12);
    append(&head, 222, 14);
    append(&head, 333, 16);
    
    print_list(head);

    free_list(&head);
    
    return 0;
}



