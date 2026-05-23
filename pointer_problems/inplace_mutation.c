#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
    int value;
    struct Node *next;
} Node;

typedef struct LinkedList
{
    struct Node *head;
} LinkedList;

LinkedList *init(void)
{
    LinkedList *ll = calloc(1, sizeof(LinkedList));
    if(ll == NULL) exit(1);
    ll->head = NULL;
    return ll;
}

void add(LinkedList *list, int value)
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
        printf("Node[%d] = %d\n", count, current->value);
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

LinkedList *reverse_list(LinkedList *list)
{
    if(list->head == NULL) return list;
    Node *current = list->head;
    Node *prev = NULL;
    Node *next = NULL;

    while(current != NULL)
    {
        next = current->next;
        current->next = prev;  
        prev = current;
        current = next;
    }

    list->head = prev;
    return list;
}

int main(void)
{
    LinkedList *list = init();
    add(list, 1);
    add(list, 2);
    add(list, 3);

    print(list);

    LinkedList *reversed = reverse_list(list);
    print(reversed);

    clean(list);

    return 0;
}
