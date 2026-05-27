#include <stdio.h>
#include <stdlib.h>

#define VERTEX 6

typedef struct EdgeNode
{
    int target_task;
    struct EdgeNode *next;
} EdgeNode;

typedef struct Graph
{
    int num_tasks;
    int *DSU; 
    EdgeNode **adj_list;
} Graph;

/**
 * @brief Finds the root parent of a given node.
 * @param idx The node to find.
 * @param union_set The DSU array.
 * @return The index of the root node.
 */
int find(int idx, Graph *graph)
{
    if(idx < 0 || idx >= graph->num_tasks) return -1;

    int root = idx;
    while(graph->DSU[root] >= 0) root = graph->DSU[root];
    return root;
}

/**
 * @brief Unions two nodes into the same set using Union by Size.
 * @param node_a A node in the first set.
 * @param node_b A node in the second set.
 * @param union_set The DSU array.
 * @return void
 */
void set_union(int node_a, int node_b, Graph *graph)
{
    // Find the actual roots of both nodes
    int root_a = find(node_a, graph);
    int root_b = find(node_b, graph);

    // If they share the same root, they are already connected! (Cycle detected)
    if(root_a == root_b)
    {
        fprintf(stderr, "Cycle detected between %d and %d", root_a, root_b);
        return;
    }

    // Union by Size (The negative number holds the size of the tree)
    // Remember: -5 is "smaller" than -2 in math, but means a BIGGER tree in DSU!
    if(graph->DSU[root_a] <= graph->DSU[root_b])
    {
        // Tree A is bigger (or equal). Attach B to A.
        graph->DSU[root_a] += graph->DSU[root_b];
        // B's parent is now A
        graph->DSU[root_b] = root_a;
    }
    else
    {
        // Tree B is bigger. Attach A to B.
        graph->DSU[root_b] += graph->DSU[root_a];
        graph->DSU[root_a] = root_b;
    }
}

/**
 * @brief Returns the number of independent cluster
 * (negative values in the DSU)
 * @param Graph graph pointer
 * @return int
 */
int get_independent_cluster(Graph *graph)
{
    int res = 0;
    for(int i = 0; i < graph->num_tasks; i++)
        if(graph->DSU[i] < 0) res++;
    
    return res;
}

/**
 * @brief Creates a new Graph and allocates spaces for the vertices
 * @param int vertices
 * @return void
 */
Graph* create_graph(int vertices)
{
    Graph *graph = (Graph *)calloc(1, sizeof(Graph));
    if(graph == NULL) exit(1);
    graph->num_tasks = 0;
    
    graph->adj_list = (EdgeNode **)calloc(vertices, sizeof(EdgeNode *));
    if(graph->adj_list == NULL) exit(1);
    
    graph->num_tasks = vertices;

    graph->DSU = (int *)malloc(vertices * sizeof(int));
    if(graph->DSU == NULL) exit(1);
    for(int i = 0; i < vertices; i++) graph->DSU[i] = -1;
    
    return graph;
}

/**
 * @brief Add a dependency to a vertex 
 * @param Graph graph pointer
 * @param int task 
 * @param int prerequisite
 * @return void
 */
void add_dependency(Graph* graph, int task, int prerequisite)
{
    EdgeNode *new_edge = malloc(sizeof(EdgeNode));
    if(new_edge == NULL) exit(1);
    
    new_edge->target_task = prerequisite;
    
    // set the new_edge next to be the current head at given index
    new_edge->next = graph->adj_list[task];
    // set the new head to be the new_edge
    graph->adj_list[task] = new_edge;  
    printf("invisible stitching phase: source %d -> target %d\n", task, prerequisite);
    set_union(task, prerequisite, graph);
}

/**
 * @brief Prints out the current Graph
 * @param Graph graph pointer
 * @return void
 */
void print(Graph *graph)
{
    for(int i = 0; i < graph->num_tasks; i++)
    {
        EdgeNode *current_node = graph->adj_list[i];
        
        while(current_node != NULL)
        {
            printf("[%d] = %d\n", i, current_node->target_task);
            current_node = current_node->next;
        }
    }
}

/**
 * @brief Free the allocated heap 
 * @param Graph graph pointer
 * @return void
 */
void clean(Graph *graph)
{
    for(int i = 0; i < graph->num_tasks; i++)
    {
        EdgeNode *current_node = graph->adj_list[i];
        EdgeNode *next_node = NULL;

        while(current_node != NULL)
        {
            next_node = current_node->next;
            free(current_node);
            current_node = next_node;
        }

    }

    free(graph->adj_list);
    free(graph->DSU);
    free(graph);
}

int main(void)
{
    int task_sub1[3] = {0,1,2};
    int task_sub2[3] = {3,4,5};
    Graph *graph = create_graph(VERTEX);

    add_dependency(graph, task_sub1[1], task_sub1[0]);
    add_dependency(graph, task_sub1[2], task_sub1[1]);

    add_dependency(graph, task_sub2[1], task_sub2[0]);
    add_dependency(graph, task_sub2[2], task_sub2[0]);
    
    print(graph);

    int independent = get_independent_cluster(graph);
    printf("# of independent cluster is %d\n", independent);

    clean(graph);
    
    return 0;
}
