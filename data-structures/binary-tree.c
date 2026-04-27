#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct TreeNode 
{
    int value;
    struct TreeNode *left;
    struct TreeNode *right;
};

/**
 * @brief Factory method used to create the instance
 * of the Tree
 * @param int value
 * @return struct TreeNode pointer
 */
struct TreeNode* tree_create_node(int value)
{
   struct TreeNode *bt = (struct TreeNode *)calloc(1, sizeof(struct TreeNode));
   if(bt == NULL) exit(1);
   bt->value = value;
   bt->left = NULL;
   bt->right = NULL;
   return bt;
}

/**
 * @brief Insert a new value in the tree
 * @param root struct TreeNode pointer
 * @param value int value to insert
 * @return struct TreeNode pointer
 */
struct TreeNode* tree_insert(struct TreeNode *root, int value)
{
    // 1. Base Case: Found the empty spot!
    if(root == NULL)    
    { 
        struct TreeNode *node = (struct TreeNode *)malloc(sizeof(struct TreeNode));
        if(node == NULL) exit(1);
        
        node->value = value;
        node->left = NULL;
        node->right = NULL;
        return node;
    }

    // 2. Recursive Step: Go Right
    if(value > root->value)
    {
        root->right = tree_insert(root->right, value);
    }
    // 3. Recursive Step: Go Left
    else if (value < root->value) // Added 'else if' to safely ignore duplicate values
    {
        root->left = tree_insert(root->left, value); // <-- FIXED WIRING
    }

    // 4. Return the unaltered root pointer back up the chain
    return root;
}

/**
 * @brief Prints out the Tree's node values in order
 * @param struct TreeNode pointer
 * @return void
 */
void tree_print_inorder(struct TreeNode *root)
{
    if (root == NULL) return; // Base case: end of the branch
    
    tree_print_inorder(root->left);
    printf("%d ", root->value);
    tree_print_inorder(root->right);
}

/**
 * @brief Remove the tree and its nodes from memory
 * @param root struct TreeNode pointer
 * @return void
 */
void tree_free(struct TreeNode *root)
{
    if(root == NULL) return;
    
    tree_free(root->left);
    tree_free(root->right); 
    free(root);
}

int main(void)
{
    struct TreeNode *bt = tree_create_node(14);
    
    struct TreeNode *new_node1 = tree_insert(bt, 3); 
    struct TreeNode *new_node2 = tree_insert(bt, 2); 
    struct TreeNode *new_node3 = tree_insert(bt, 5);
    struct TreeNode *new_node4 = tree_insert(bt, 6);
    struct TreeNode *new_node5 = tree_insert(bt, 4);
    
    tree_print_inorder(bt);
    tree_free(bt);
    
    return 0;
}
