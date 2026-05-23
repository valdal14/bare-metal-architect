#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
    char value;
    struct Node *next;
} Node;


typedef struct LinkedList
{
    struct Node *head;
} LinkedList;

void init(LinkedList **list)
{
    LinkedList *ll = calloc(1, sizeof(LinkedList));
    if(ll == NULL) exit(1);
    ll->head = NULL;
    *list = ll;
}

void add(LinkedList *list, char value)
{
    Node *node = malloc(sizeof(Node));
    if(node == NULL) exit(1);
    node->value = value;
    node->next = NULL;

    if(list->head == NULL)
    {
        list->head = node;
    }
    else
    {
        Node *current = list->head;
        while(current->next != NULL) current = current->next;
        current->next = node;
    }
}

void print(LinkedList *list)
{
    if(list->head == NULL) return;
    Node *current = list->head;
    int count = 0;

    while(current != NULL)
    {
        printf("Node[%d] = %c\n", count, current->value);
        current = current->next;
        count++;
    }
    printf("-----------\n");
}

void clean(LinkedList *list)
{
    if(list->head == NULL) return;
    Node *current = list->head;
    Node *next_node = NULL;

    while(current != NULL)
    {
        next_node = current->next;
        free(current);
        current = next_node;
    }

    free(list);
}


void merge_alternating(LinkedList *list1, LinkedList *list2)
{
    if(list1->head == NULL || list2->head == NULL) return;

    Node *c1 = list1->head;
    Node *c2 = list2->head;
    Node *l1_next = NULL;
    Node *l2_next = NULL;
    
    while(c1 != NULL && c2 != NULL)
    {
        l1_next = c1->next;
        l2_next = c2->next;
        c1->next = c2;
        c2->next = l1_next;
        c1 = l1_next;
        c2 = l2_next;
    }
}

int main(void)
{
    LinkedList *ll1 = NULL;
    LinkedList *ll2 = NULL;
    init(&ll1);
    init(&ll2);

    add(ll1, '1');
    add(ll1, '2');
    add(ll1, '3');
    print(ll1);

    add(ll2, 'A');
    add(ll2, 'B');
    add(ll2, 'C');
    print(ll2);

    merge_alternating(ll1, ll2);
    print(ll1);

    clean(ll1);
    free(ll2);
    return 0;
}
