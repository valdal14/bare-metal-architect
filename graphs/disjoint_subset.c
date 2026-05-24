#include <stdio.h>
#include <stdlib.h>

#define UNION_SET_SIZE 11

/**
 * @brief Initializes the Disjoint Set. 
 * Every node starts as its own root with a size of 1 (represented as -1).
 * @param int union_set
 */
void init_dsu(int union_set[UNION_SET_SIZE])
{
    for(int i = 0; i < UNION_SET_SIZE; i++)
        union_set[i] = -1; 
}

/**
 * @brief Finds the root parent of a given node.
 * @param idx The node to find.
 * @param union_set The DSU array.
 * @return The index of the root node.
 */
int find(int idx, int union_set[UNION_SET_SIZE])
{
    if(idx < 0 || idx >= UNION_SET_SIZE) return -1;

    int root = idx;
    while(union_set[root] >= 0) root = union_set[root];
    return root;
}

/**
 * @brief Unions two nodes into the same set using Union by Size.
 * @param node_a A node in the first set.
 * @param node_b A node in the second set.
 * @param union_set The DSU array.
 * @return void
 */
void set_union(int node_a, int node_b, int union_set[UNION_SET_SIZE])
{
    // Find the actual roots of both nodes
    int root_a = find(node_a, union_set);
    int root_b = find(node_b, union_set);

    // If they share the same root, they are already connected! (Cycle detected)
    if(root_a == root_b) 
    {
        fprintf(stderr, "Cycle detected between %d and %d", root_a, root_b);
        exit(1);
    }

    // 2. Union by Size (The negative number holds the size of the tree)
    // Remember: -5 is "smaller" than -2 in math, but means a BIGGER tree in DSU!
    if(union_set[root_a] <= union_set[root_b]) 
    {
        // Tree A is bigger (or equal). Attach B to A.
        union_set[root_a] += union_set[root_b]; // Combine their sizes (-5 + -2 = -7)
        // B's parent is now A
        union_set[root_b] = root_a;             
    }
    else 
    {
        // Tree B is bigger. Attach A to B.
        union_set[root_b] += union_set[root_a]; 
        union_set[root_a] = root_b;             
    }
}

int main(void)
{
    int u[UNION_SET_SIZE];
    init_dsu(u);

    // Build Set A {3, 5, 9} by unioning them one by one
    set_union(3, 5, u);
    set_union(3, 9, u);
    // set_union(5, 9, u); This will provoce a cycle

    // Build Set B {4, 7, 8, 10}
    set_union(4, 7, u);
    set_union(4, 8, u);
    set_union(4, 10, u);

    // Now, let's union Set A and Set B together!
    // We just pass any node from A and any node from B.
    set_union(9, 8, u); 

    int parent = find(9, u);
    printf("The root parent of idx 9 is = %d\n", parent); 
    // Output will be 4, because Set B was larger, so 4 absorbed 3!

    return 0;
}
