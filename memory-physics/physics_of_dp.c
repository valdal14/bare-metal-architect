#include <stdio.h>
#include <stdlib.h>

struct Node 
{
    int data;
    struct Node *next;
};

void remove_first_node(struct Node **head_ref)
{
    // 1. Check if the list is empty
    if (*head_ref == NULL) return;

    // 2. HOLD THE FIRST BOX. 
    // We create a temporary sticky note and copy the address of the first box onto it.
    // If we don't do this, we will lose the box forever when we change the head!
    struct Node *temp = *head_ref;

    // 3. SHIFT THE HEAD TO THE SECOND BOX.
    // We look inside the first box to find the address of the second box (temp->next).
    // We take our eraser, go back to main (*head_ref), and overwrite it with the second box's address.
    *head_ref = temp->next;

    // 4. DESTROY THE FIRST BOX.
    // The list in main is now safely pointing to the second box. 
    // We can safely blow up the old first box using our temporary sticky note.
    free(temp);
}

void insert_at_front(struct Node **head_ref, int new_data)
{
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    if(new_node == NULL) exit(1);
    new_node->data = new_data;
    new_node->next = NULL;

    // assign to the old head the address of the head_ref current head
    struct Node *old_head = *head_ref;
    // assign to head_ref head the new_node (new head)
    *head_ref = new_node;
    // assign the old_head to the new_node next so it the second node...
    new_node->next = old_head;

    printf("List new Head: %d\n", head_ref[0]->data);
    printf("List new Next: %d\n", head_ref[0]->next->data);

}

int main(void)
{
    // Build Box 1
    struct Node *box1 = (struct Node *)malloc(sizeof(struct Node));
    box1->data = 7;
    
    // Build Box 2
    struct Node *box2 = (struct Node *)malloc(sizeof(struct Node));
    box2->data = 14;
    box2->next = NULL;

    // Connect Box 1 to Box 2
    box1->next = box2;

    // Point our main list tracker to Box 1
    struct Node *list = box1;

    printf("List head before: %d\n", list->data); // Prints 7

    // Pass the GPS coordinates of the 'list' sticky note
    remove_first_node(&list);
    
    printf("List head after: %d\n", list->data);  // Prints 14

    insert_at_front(&list, 21);

    free(list); // Clean up the remaining box
    return 0;
}
