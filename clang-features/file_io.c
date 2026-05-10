#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node
{
    int data;
    struct Node *next;
};

struct LinkedList
{
    struct Node *head;
    struct Node *tail;
    int count;
};

/**
 * @brief Init a new LinkedList
 * @param struct LinkedList list_ref double pointer
 * @return void
 */
void init(struct LinkedList **list_ref)
{
    struct LinkedList *ll = (struct LinkedList *)calloc(1, sizeof(struct LinkedList));
    if(ll == NULL) exit(1);
    ll->head = NULL;
    ll->tail = NULL;
    ll->count = 0;

    *list_ref = ll;
}

/**
 * @brief Adds a new node to the LinkedList
 * @param struct LinkedList double pointer
 * @param int value
 * @return void
 */
void add_node(struct LinkedList **list_ref, int value)
{
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    if(new_node == NULL) exit(1);
    new_node->data = value;
    new_node->next = NULL;

    // increment the LinkedList count
    list_ref[0]->count += 1;

    if(list_ref[0]->head == NULL)
    {
        list_ref[0]->head = new_node;
        list_ref[0]->tail = new_node;
    }
    else
    {
        list_ref[0]->tail->next = new_node;
        list_ref[0]->tail = new_node;
    }
}

/**
 * @brief Deallocate the LinkedList and its nodes
 * @param struct LinkedList list_ref double pointer
 * @return void
 */
void free_list(struct LinkedList **list_ref)
{
    if (*list_ref == NULL) return; // Safety check

    struct Node *current = (*list_ref)->head;
    struct Node *next_node = NULL;
    
    while(current != NULL)
    {
        next_node = current->next;
        free(current);
        current = next_node;
    }

    free(*list_ref);
    
    *list_ref = NULL;
}

/**
 * @brief Prints the LinkedList
 * @param struct LinkedList list pointer
 * @return void
 */
void print(struct LinkedList *list)
{
    if(list->count == 0)
    {
        printf("The LinkedList is empty, there is nothing to print\n");
    }
    else
    {
        struct Node *current = list->head;
        char *msg = list->count > 1 ? "nodes" : "node";
        int count = 0;

        printf("The LinkedList holds %d %s\n", list->count, msg); 
        printf("- Head value = %d\n", list->head->data);
        printf("- Tail value = %d\n", list->tail->data);
    
        while(current != NULL)
        {
            printf("Node[%d] = %d\n", count, current->data);
            current = current->next;
            count++;
        }
 
        printf("------------------------------\n");
    }
}

/**
 * @brief Saves the values stored in the LinkedList
 * @param LinkedList list_ref double pointer
 * @param const char filename pointer
 * @return void
 */
void save_list(struct LinkedList **list_ref, const char *filename)
{
    if(list_ref[0]->count == 0)
    {
        printf("There is nothing to save from memory to disk\n");
        exit(1);
    }

    FILE *file = fopen(filename, "w");
    if(file == NULL) exit(1);
    
    printf("\nSaving LinkedList on Disk, please wait...\n");
    
    struct Node *current = list_ref[0]->head;
    
    int count = 0;
    
    while(current != NULL)
    {
        fprintf(file, "Node[%d] =  %d\n", count, current->data);
        current = current->next;
        count++;
    }

    fclose(file);
    printf("- Data was successfully saved on Disk\n");
}

/**
 * @brief Loads the data from the disk and fills up
 * the in-memory LinkedList
 * @param LinkedList list_ref double pointer
 * @param const char filename pointer
 * @return void
 */
void load_list(struct LinkedList **list_ref, const char *filename)
{
    printf("\nLoading LinkedList on Disk, please wait...\n");

    FILE *file = fopen(filename, "r");
    if(file == NULL) exit(1);
   
    // init the LinkedList
    init(list_ref);
    // crate a buffer to store the value 
    int value = 0; 
    while (fscanf(file, "%*s %*s %d ", &value) == 1)
    {
        printf("Read number:  %d\n", value);
        add_node(list_ref, value);
    }
    
    printf("------------------------------\n");

    fclose(file);
}

int main(void)
{
    char *filename = "./clang-features/data.txt";
    struct LinkedList *ll = NULL;
    // init a list
    init(&ll);
    // add nodes 
    add_node(&ll, 14);
    print(ll);
    add_node(&ll, 7); 
    print(ll);
    add_node(&ll, 18); 
    print(ll);
    add_node(&ll, 3); 
    print(ll);
    
    // save LinkedList
    save_list(&ll, filename);

    // free the list 
    free_list(&ll);

    // load LinkedList from disk
    load_list(&ll, filename);
    print(ll);
    free_list(&ll);
    
    return 0;
}
