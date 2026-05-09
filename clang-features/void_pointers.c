#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT 0
#define STR 1

typedef struct Node
{
    void *data;
    int type;
    struct Node *next;
} Node;


/**
 * @brief Validates the given input value using the supported types
 * @param void pointer 
 * @param int type 
 * @return void pointer
 */
void *validate_input(void *input, int type)
{
    switch(type)
    {
        case 0:
            if((int *)input != NULL)
            {
                return input;                               
            }
            else
            {
                fprintf(stderr, "NULL int input value\n");
                exit(1);
            }
        case 1:
            if((char *)input != NULL)
            {
                return input;
            }
            else
            {
                fprintf(stderr, "NULL string input value\n");
                exit(1);
            }
        default:
            fprintf(stderr, "Unsupported type\n");
            exit(1);
    }
}

/**
 * @brief Callback that perform the add node operation
 * @param Node list double pointer
 * @param Node new_node pointer 
 * @return void
 */
void _on_completion(Node **list_ref, Node *new_node)
{

    if(*list_ref == NULL) 
    {
        *list_ref = new_node;
    }
    else
    {
        Node *current = *list_ref;
        while(current->next != NULL)
            current = current->next;

        current->next = new_node;
    }
}

/**
 * @brief Add a new Node to the list
 * @param Node head_ref double pointer
 * @param void new_data pointer
 * @param int type Must be either INT or STR
 * @return void
 */
void add_node(Node **head_ref, void *new_data, int type, void(*callback)(Node **list_ref, Node *new_node))
{

    Node *node = (Node *)malloc(sizeof(Node));
    if(node == NULL) exit(1);
    node->next = NULL;
    node->type = type;

    switch(type)
    {
        case 0:
        {
            int *int_val = validate_input(new_data, type);
            node->data = (int *)malloc(sizeof(int));
            *(int *)(node->data) = *int_val;
            callback(head_ref, node);
            break;
        }
        case 1:
        {   
            // get the pointer back
            char *str_val = validate_input(new_data, type);
            // get the string size + the null terminator
            int str_size = strlen((char *)str_val) + 1;
            // cast back to string from void 
            char *string = (char *)new_data;
            
            node->data = (char *)malloc(str_size);
            strncpy(node->data, string, str_size);

            callback(head_ref, node);
            break;
        }
        default:
            fprintf(stderr, "Cannot add an unsupported type as new node\n");
            exit(1);
    }
}

/**
 * @brief Prints the List and their generic data 
 * @param Node list double pointer
 * @return void
 */
void print_list(Node *list)
{
    Node *current = list;
    int count = 0;

    while(current != NULL)
    {
        if(current->type == 0) printf("[%d] int value = %d\n", count, *(int *)current->data);
        if(current->type == 1) printf("[%d] str value = %s\n", count, (char *)current->data);
        count++;
        current = current->next;
    }
}

/**
 * @brief Deallocates the Nodes and set the list back to NULL
 * @param Node list double pointer
 * @return void
 */
void free_list(Node **list)
{
    Node *current = *list;
    Node *next_node = NULL;

    while(current != NULL)
    {
        next_node = current->next;
        free(current->data);
        free(current);
        current = next_node;
    }

    *list = NULL;
}

int main(void)
{
    Node *head = NULL;
    
    char *c_val = "Hello World";
    add_node(&head, c_val, STR, _on_completion);
    
    int i_val = 18;
    add_node(&head, &i_val, INT, _on_completion);

    print_list(head);

    free_list(&head);
    return 0;
}
