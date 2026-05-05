#include <stdio.h>
#include <stdlib.h>

struct Node 
{
    int data;
    struct Node *next;
};

/**
 * @brief Finds and returns the middle node of a Singly Linked List.
 *        If there is an even number of nodes, returns the second middle node.
 *        Must be completed in a single pass (O(n) time, O(1) space).
 * @param head Pointer to the head of the linked list.
 * @return struct Node* Pointer to the middle node, or NULL if list is empty.
 */
struct Node* find_middle(struct Node *head)
{
    // Protect against empty lists
    if (head == NULL) {
        return NULL;
    }

    struct Node *slow = head;
    struct Node *fast = head;

    // Fast pointer moves 2 steps, slow moves 1 step.
    while (fast != NULL && fast->next != NULL) 
    {
        slow = slow->next;          
        fast = fast->next->next;   
    }

    // When the hare reaches the end, the tortoise is exactly in the middle.
    return slow;
}

/**
 * @brief Init a new LL 
 * @return 0;
 */
struct Node *init(void)
{
    struct Node *head = (struct Node *)calloc(1, sizeof(struct Node));
    if(head == NULL) exit(1);
    head->data = 1;
    return head;
}

/**
 * @brief Add a new node to the LL
 * @param struct Node Pointer
 * @param int val 
 * @return void
 */
void add_node(struct Node *head, int val)
{
    struct Node *new_node = (struct Node *)calloc(1, sizeof(struct Node));
    if(new_node == NULL) exit(1);
    new_node->data = val;

    struct Node *current = head;

    while(current->next != NULL)
    {
        current = current->next;
    }

    new_node->next = NULL;
    current->next = new_node;
}

/**
 * @brief Prints the LL 
 * @param struct Node Pointer
 * @return void
 */
void  print_nodes(struct Node *head)
{
    struct Node *node = head;

    while(node != NULL)
    {
        printf("%d\n", node->data);
        node = node->next;
    }

    printf("-----------------\n");
}

/**
 * @brief remove objects from memory
 * @param struct Node Pointer
 * @return void
 */
void free_nodes(struct Node *head)
{
    struct Node *current = head;
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
    struct Node *head = init();
    add_node(head, 4);
    add_node(head, 7);
    add_node(head, 15);
    add_node(head, 17);
    print_nodes(head);
    
    struct Node node = *find_middle(head);
    printf("Mid node value = %d\n", node.data);
    free_nodes(head);
    return 0;
}
