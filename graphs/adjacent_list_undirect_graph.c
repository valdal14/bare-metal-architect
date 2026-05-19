#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 5

struct Edge
{
    int destination; 
    struct Edge *next;
};

struct Graph
{
    struct Edge **head;
    int num_vertices;
};

/**
 * @brief Initializes a new Graph with zeroed memory.
 * @return struct Graph The initialized graph instance.
 */
struct Graph init(void)
{
    struct Graph graph;
    
    struct Edge **head_array = (struct Edge **)calloc(MAX_VERTICES, sizeof(struct Edge *));
    
    if(head_array == NULL) 
    {
        printf("Fatal: Memory allocation failed.\n");
        exit(1);
    }
    
    graph.head = head_array;
    graph.num_vertices = MAX_VERTICES;
    return graph;
}

/**
 * @brief Adds a new directed edge from a source vertex to a destination vertex.
 * @param graph Pointer to the graph instance.
 * @param source The index of the origin vertex.
 * @param destination The index of the target vertex.
 * @return void
 */
void add_edge(struct Graph *graph, int source, int destination)
{
    // Protect against out-of-bounds indices
    if(source >= graph->num_vertices || source < 0) return;
    if(destination >= graph->num_vertices || destination < 0) return;

    struct Edge *new_edge = malloc(sizeof(struct Edge));
    if(new_edge == NULL) exit(1);
    
    new_edge->destination = destination;
    new_edge->next = NULL;
    
    new_edge->next = graph->head[source];
    graph->head[source] = new_edge;
}

/**
 * @brief Shows the edges/links of a given vertex.
 * @param graph Pointer to the graph instance.
 * @param vertex The index of the vertex to inspect.
 * @return void
 */
void show_vertex_connection(struct Graph *graph, int vertex)
{
    if(vertex >= graph->num_vertices || vertex < 0) return;
    
    struct Edge *current = graph->head[vertex];

    printf("Neighbors of Vertex %d:\n", vertex);
    while(current != NULL)
    {
        printf(" -> %d\n", current->destination);
        current = current->next;
    }
    printf("---------------------------------------------\n");
}

/**
 * @brief Prints out a representation of the Graph with vertexes and edges.
 * @param graph Pointer to the graph instance.
 * @return void
 */
void show_graph(struct Graph *graph)
{
    for(int i = 0; i < graph->num_vertices; i++)
    {
        struct Edge *current = graph->head[i];

        printf("Vertex %d: ", i);
        while(current != NULL)
        {
            printf("%d -> ", current->destination);
            current = current->next;
        }
        printf("NULL\n"); 
    }
    printf("---------------------\n");
}

/**
 * @brief Frees all dynamically allocated memory for the graph.
 * @param graph Pointer to the graph instance.
 * @return void
 */
void free_graph(struct Graph *graph)
{
    for(int i = 0; i < graph->num_vertices; i++)
    {
        struct Edge *current = graph->head[i];
        struct Edge *next_node = NULL;

        while(current != NULL)
        {
            next_node = current->next;
            free(current);
            current = next_node;
        }
    }

    free(graph->head);
}

int main(void)
{
    struct Graph adj_list = init();
    
    // Add Edges to vertex 0
    add_edge(&adj_list, 0, 3);
    add_edge(&adj_list, 0, 4);
    
    // Add Edges to vertex 1
    add_edge(&adj_list, 1, 3);
    
    // Add Edges to vertex 2
    add_edge(&adj_list, 2, 2);
    add_edge(&adj_list, 2, 4);
    
    show_vertex_connection(&adj_list, 0);
    show_graph(&adj_list);

    free_graph(&adj_list);
    return 0;
}
