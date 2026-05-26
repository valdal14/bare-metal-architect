#include <stdio.h>
#include <stdlib.h>

#define VERTEX 4

typedef struct EdgeNode
{
    int target_task;
    struct EdgeNode *next;
} EdgeNode;

typedef struct Graph
{
    int num_tasks;
    EdgeNode **adj_list;
} Graph;

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
    free(graph);
}

int main(void)
{
    Graph *graph = create_graph(VERTEX);
    add_dependency(graph, 1, 0);
    add_dependency(graph, 2, 1);
    add_dependency(graph, 3, 1);
    add_dependency(graph, 3, 2);

    print(graph);
    
    return 0;
}
