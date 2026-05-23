#include <stdio.h>
#include <stdlib.h>

struct Node 
{
    int value;
    struct Node *next;
};

struct LinkedList
{
    struct Node *head;
};

void init_list(struct LinkedList **list)
{
    struct LinkedList *ll = (struct LinkedList *)calloc(1, sizeof(struct LinkedList));
    if(ll == NULL) exit(1);
    ll->head = NULL;
    list[0] = ll;
}

void add_node(struct LinkedList *ll, int value)
{
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    if(node == NULL) exit(1);
    node->value = value;
    node->next = NULL;

    if(ll->head == NULL)
    {
        ll->head = node;
    }
    else
    {
        struct Node *current = ll->head;
        while(current->next != NULL) current = current->next;
        current->next = node;
    }

}

void free_ll(struct LinkedList *list)
{
    struct Node *current = list->head;
    struct Node *next_node = NULL;

    while(current != NULL)
    {
        next_node = current->next;
        free(current);
        current = next_node;
    }

    free(list);
    list = NULL;
}

void print(struct LinkedList *list)
{
    if(list->head == NULL) return;

    int count = 0;
    struct Node *current = list->head;
    while(current != NULL)
    {
        printf("Node[%d] = %d\n", count, current->value);
        current = current->next;
        count++;
    }
    printf("-----------\n");
}

struct Node* find_middle(struct LinkedList *list)
{
    if(list->head == NULL) return NULL;
    
    struct Node *slow = list->head;
    struct Node *fast = list->head;

    while (fast != NULL && fast->next != NULL) 
    {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    return slow;
}



int main(void)
{
    struct LinkedList *ll = NULL;
    init_list(&ll);

    add_node(ll, 1);
    add_node(ll, 2);
    add_node(ll, 3);
    add_node(ll, 4);
    add_node(ll, 5);
    print(ll);

    struct Node *middle = find_middle(ll);
    printf("Middle node value is %d\n", middle->value);
    
    free_ll(ll);
    return 0;
}
