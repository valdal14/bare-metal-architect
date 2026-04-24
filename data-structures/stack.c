#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USER_LENGTH 10
#define STACK_CAPACITY 5

struct User
{
    char name[USER_LENGTH];
    int age;
};

struct Stack
{
    struct User *users[STACK_CAPACITY];
    int size;
};

/**
 * @brief Factory method to create and allocate 
 * space in memory for the stack
 * @return struct Stack  Pointer
 */
struct Stack *init_stack(void)
{
    struct Stack *stack = (struct Stack *)calloc(1, sizeof(struct Stack));
    if(stack == NULL) exit(1); 
    stack->size = 0;
    return stack;
}

/**
 * @brief Push a new User on top of the stack
 * @param struct Stack Pointer
 * @param struct User Pointer
 * @return void
 */
void push(struct Stack *stack, char *name, int name_length, int age)
{    
    if((stack->size + 1) >= STACK_CAPACITY)
    {
        fprintf(stderr, "Stack max capacity (%d) reached, please pop before pushing new item\n", STACK_CAPACITY);
        exit(1);
    }

    if(name_length >= USER_LENGTH)
    {
        fprintf(stderr, "The name of the user is too long, max length allowed = %d\n", USER_LENGTH);
        exit(1);
    }

    struct User *user = (struct User *)calloc(1, sizeof(struct User));
    if(user == NULL) exit(1);

    user->age = age;
    // first assign the user to the stack -> users
    stack->users[stack->size] = user;
    // copy the name 
    strncpy(stack->users[stack->size]->name, name, name_length * sizeof(char));
    stack->size = stack->size + 1;
}

/**
 * @brief Pop the first element from the stack 
 * @param struct Stack Pointer
 * @return struct User Pointer
 */
struct User *pop(struct Stack *stack)
{
    if(stack->size == 0)
    {
        fprintf(stderr, "Cannot pop an empty stack\n");
        exit(1);
    }

    struct User *root = (struct User *)calloc(1, sizeof(struct User));
    if(root == NULL) exit(1);
   
    memcpy(root, stack->users[stack->size - 1], sizeof(struct User));
    free(stack->users[stack->size - 1]);
    stack->size = stack->size - 1;
    
    return root;
}


/**
 * @brief Print out the current stack
 * @param struct Stack Pointer
 * @return void
 */
void print(struct Stack *stack)
{
    printf("\n--- Current Stack ---------------\n");

    for(int i = stack->size; i > 0; i--)
        printf("%s is %d years old\n", stack->users[i - 1]->name, stack->users[i - 1]->age);

    printf("---------------------------------\n");
    printf("Stack size = %d\n", stack->size);
    printf("---------------------------------\n\n");
}

/**
 * @brief Free the objects stored in the heap 
 * @param struct Stack Pointer
 * @return void
 */
void free_alloc(struct Stack *stack)
{
    for(int i = 0; i < stack->size; i++)
        free(stack->users[i]);

    free(stack);
}

int main(void)
{
    struct Stack *stack = init_stack();

    push(stack, "Grazia", 6, 6); 
    push(stack, "Leo", 3, 50); 
    push(stack, "Val", 3, 47);
    push(stack, "Milu", 4, 21);

    print(stack);
    
    struct User *user = pop(stack);
    printf("%s with age %d was removed from the stack\n", user->name, user->age); 
    
    print(stack);
    
    free_alloc(stack);
    return 0;
}
