#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
    int value;
    struct Node *next;
} Node;

void init_list(Node **list)
{
    Node *node = (Node *)malloc(sizeof(Node));
    if(node == NULL) exit(1);
    node->value = 1;
    node->next = NULL;
    *list = node;
}

void add_node(Node **node, int value)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    if(new_node == NULL) exit(1);
    new_node->value = value;
    new_node->next = NULL;

    Node *current = node[0];
    while(current->next != NULL) current = current->next;

    current->next = new_node;
}

void remove_node(struct Node **head, int target_value)
{
    if (head[0] == NULL) return;

    Node *current = head[0];
    Node *prev = NULL;

    // The target is the head itself
    if (current->value == target_value)
    {
        head[0] = current->next; 
        free(current);
        return;
    }

    
    // Walk down the list. Stop when we find the value OR hit the end.
    while (current != NULL && current->value != target_value)
    {
        prev = current;          
        current = current->next;
    }

    // If current became NULL, it means we searched the whole list and didn't find it
    if (current == NULL) return;
    
    // Stitch the chain around the target node
    prev->next = current->next;
    
    free(current); 
}

void print(Node *list)
{
    Node *current = list;
    int count = 0;

    while(current != NULL)
    {
        printf("Node[%d] = %d\n", count, current->value);
        current = current->next;
        count++;
    }

    printf("---------------------\n");
}

void free_list(Node *list)
{
    Node *current = list;
    Node *next_node = NULL;

    while(current != NULL)
    {
        next_node = current->next;
        free(current);
        current = next_node;
    }

    free(current);
    list = NULL;
}

int main(void)
{
    struct Node *list = NULL;
    init_list(&list);
    
    add_node(&list, 2); 
    add_node(&list, 3); 
    add_node(&list, 4); 
    print(list);
    // remove the node 
    remove_node(&list, 2);
    print(list);

    free_list(list);
    return 0;
}
