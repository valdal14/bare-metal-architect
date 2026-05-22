#include <stdio.h>
#include <stdlib.h>

struct Node 
{
    int value;
    struct Node *next;
};

struct Node *init_ll(void)
{
    struct Node *ll = (struct Node *)malloc(sizeof(struct Node));
    if(ll == NULL) exit(1);

    ll->value = 1;
    ll->next = NULL;

    return ll;
}

void add_node(struct Node *ll, int value)
{    
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    if(new_node == NULL) exit(1);
    
    new_node->value = value;
    new_node->next = NULL;
    
    struct Node *current = ll;

    while(current->next != NULL) current = current->next;
   
    current->next = new_node;
}

void print(struct Node *ll)
{
    struct Node *current = ll;
    while(current != NULL)
    {
        printf("Value = %d\n", current->value);
        current = current->next;
    }
}

void free_ll(struct Node *ll)
{
    struct Node *current = ll;
    struct Node *next_node = NULL;

    while(current != NULL)
    {
        next_node = current->next;
        free(current);
        current = next_node;
    }

    free(current);
}

int main(void)
{
    struct Node *ll = init_ll();
    add_node(ll, 2);
    add_node(ll, 3);
    add_node(ll, 4);
    
    //TODO reorder_list(ll);

    print(ll);
    free_ll(ll);

    return 0;
}
