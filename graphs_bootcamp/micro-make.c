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
    int *state;
    int *topo_order;
    int topo_idx;
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
        fprintf(stderr, "Cycle detected between %d and %d\n", root_a, root_b);
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
 * @brief Detects if there is a cycle
 * @param Graph graph pointer
 * @param int current_task 
 * @return int 
 */
int has_cycle(Graph *graph, int current_task)
{

    if(current_task < 0 || current_task > graph->num_tasks) exit(1);
    // check if the current_task has a cycle
    if(graph->state[current_task] == 1) return 1;
    // if current_task is 2 it means we already visited and no cycles were detected 
    if(graph->state[current_task] == 2) return 0;

    if(graph->state[current_task] == 0)
    {
        printf("Current task = %d\n", current_task);
        graph->state[current_task] = 1;

        EdgeNode *current = graph->adj_list[current_task];
             
        while(current != NULL)
        {
            if(has_cycle(graph, current->target_task) == 1)
            {
                printf("Cycle detected: current_task %d & task %d\n", current_task, current->target_task);
                return 1;
            }

            current = current->next;
        }
        
        // set the state to visited 
        graph->state[current_task] = 2;
        // add the current_task to the topology order stack
        graph->topo_order[graph->topo_idx] = current_task;
        graph->topo_idx++;

        return 0;
    }

    exit(1);
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
    
    graph->topo_order = (int *)malloc((vertices -1) * sizeof(int));
    if(graph->topo_order == NULL) exit(1);
    graph->topo_idx = 0;

    graph->DSU = (int *)malloc(vertices * sizeof(int));
    if(graph->DSU == NULL) exit(1);
    for(int i = 0; i < vertices; i++) graph->DSU[i] = -1;
    

    graph->state = (int *)malloc(vertices * sizeof(int));
    if(graph->state == NULL) exit(1);
    for(int i = 0; i < vertices; i++) graph->state[i] = 0;

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
 * @brief Testing the DAG for cycles 
 * @param Graph graph pointer
 * @return void
 */
void test_graph_for_cycles(Graph *graph)
{
    for(int i = 0; i < graph->num_tasks; i++)
    {
        // Only trigger the DFS if we haven't explored this node yet
        if(graph->state[i] == 0) 
        {
            int res = has_cycle(graph, i);
            char *string_res = res == 1 ? "true" : "false";
            printf("Task %d cycle = %s\n", i, string_res);
        }
    }

    printf("- Task Execution order:\n");
    
    for(int i = 0; i < graph->num_tasks; i++)
    {
        printf("Task[%d] -> %d ", i, graph->topo_order[i]);
    }
    printf("\n");
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
    free(graph->state);
    free(graph->topo_order);
    free(graph);
}

int main(void)
{
    int task_sub1[3] = {0,1,2};
    int task_sub2[3] = {3,4,5};
    Graph *graph = create_graph(VERTEX);

    add_dependency(graph, task_sub1[1], task_sub1[0]);
    add_dependency(graph, task_sub1[2], task_sub1[1]);
    
    //add_dependency(graph, task_sub1[1], task_sub1[2]); // cycle 

    add_dependency(graph, task_sub2[1], task_sub2[0]);
    add_dependency(graph, task_sub2[2], task_sub2[0]);
    
    print(graph);

    int independent = get_independent_cluster(graph);
    printf("# of independent cluster is %d\n", independent);
    printf("--------------------------\n");
    
    test_graph_for_cycles(graph);
    
    clean(graph);
    
    return 0;
}
