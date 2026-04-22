#include <stdio.h>
#include <stdlib.h>

struct Node
{
    int id;
    struct Node *next;
};

/**
 * @breif struct Node factory method
 * @param int value
 * @return struct Node Pointer
 */
struct Node* create_node(int value)
{
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    if(node == NULL) exit(1);
    node->id = value;
    node->next = NULL;
    return node;
}

int main(void)
{
    struct Node *a = create_node(100);
    struct Node *b = create_node(200);
    // node a points to node b
    a->next = b;
    // use node a to get to b
    printf("Node's b id = %d\n", a->next->id);
    // free allocated memory
    free(a->next);
    free(a);

    return 0;
}
