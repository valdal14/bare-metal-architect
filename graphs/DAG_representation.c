#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_CAPACITY 4
#define DAG_TYPE 0
#define VERTEX_TYPE 1

struct Vertex
{
    int value;
    struct Vertex *next;
};

struct DAG
{
    struct Vertex **vertex;
    int count;
};

/**
 * @brief Prints out the error based on the given type 
 * and exit the program
 * @param int type
 * @return void
 */
void handle_error(int type)
{
    switch(type)
    {
        case 0:
            printf("DAG could not be allocated\n");
            break;
        case 1:
            printf("Vertex could not be allocated\n");
            break;
        default:
            printf("Unsupported type\n");
            break;
    }

    exit(1);
}

/**
 * @brief Checks the allocation of a given type
 * @param void p pointer
 * @param int type
 * @return void
 */
void check_alloc(void *p, int type)
{
    switch(type)
    {
        case 0:
            if((struct DAG *)p == NULL) handle_error(type);
            break;
        case 1:
            if((struct Vertex *)p == NULL) handle_error(type);
            break;
        default:
            handle_error(-1);
            break;
    }
}

/**
 * @brief Inits a new DAG
 * @param struct DAG double pointer
 * @return void
 */
void init_dag(struct DAG **dag)
{
    struct DAG *new_dag = (struct DAG *)calloc(1, sizeof(struct DAG));
    check_alloc(new_dag, DAG_TYPE);
    struct Vertex **vertexes = (struct Vertex **)calloc(DEFAULT_CAPACITY, sizeof(struct Vertex *));
    check_alloc(vertexes, VERTEX_TYPE);
    new_dag->vertex = vertexes;
    new_dag->count = 0;
    *dag = new_dag;
}

/**
 * @brief Deallocated all vertexes of the DAG Graph
 * @param struct DAG pointer
 * @return void
 */
void free_dag(struct DAG *dag)
{
    for(int i = 0; i < dag->count; i++)
    {
        struct Vertex *current = dag->vertex[i];
        struct Vertex *next_node = NULL;

        while(current != NULL)
        {
            next_node = current->next;
            free(current);
            current = next_node;
        }
    }

    free(dag->vertex);
}

/**
 * @brief Adds a new vertex to the DAG 
 * @param int vertex_idx
 * @param int value
 */
void add_vertex(struct DAG **dag, int vertex_idx, int value)
{
    if(dag[0]->count + 1 > DEFAULT_CAPACITY)
    {
        fprintf(stderr, "Max capacity reached\n");
        exit(1);
    }

    struct Vertex *vertex = (struct Vertex *)malloc(sizeof(struct Vertex));
    check_alloc(vertex, VERTEX_TYPE);
    vertex->value = value;
    vertex->next = NULL;

    vertex->next = dag[0]->vertex[vertex_idx];
    dag[0]->vertex[vertex_idx] = vertex;
    dag[0]->count += 1;
}

/**
 * @brief Adds a new edge to an existing Vertex
 * @param struct DAG double pointer
 * @param int source_vertex
 * @param int destination_vertex
 */
void add_edge(struct DAG **dag, int source_vertex, int destination_vertex)
{
    if(source_vertex >= DEFAULT_CAPACITY || source_vertex < 0)
    {
        fprintf(stderr, "Vertex index out of bound, max idx allowed is %d\n", DEFAULT_CAPACITY - 1);
        exit(1);
    }
    
    struct Vertex *new_edge = (struct Vertex *)malloc(sizeof(struct Vertex));
    new_edge->value = destination_vertex;
    
    // Get the Vertex back from the DAG by the id 
    struct Vertex *head = dag[0]->vertex[source_vertex];
    while(head->next != NULL) head = head->next;

    new_edge->next = NULL;
    head->next = new_edge;
}

/**
 * @brief Travers the DAG Graph 
 * @param struct DAG pointer
 * @return void
 */
void DFS(struct DAG *dag)
{
    for(int i = 0; i < dag->count; i++)
    {
        struct Vertex *current = dag->vertex[i];

        while(current != NULL)
        {
            printf("%d -> ", current->value);
            current = current->next;
        }

        printf("\n");
   }
}

int main(void)
{
    struct DAG *dag = NULL; 
    init_dag(&dag);
    // add vertexes
    add_vertex(&dag, 0, 1);
    add_vertex(&dag, 1, 2);
    add_vertex(&dag, 2, 3);
    add_vertex(&dag, 3, 4);
    // add edges 
    add_edge(&dag, 0, 1);
    add_edge(&dag, 0, 3);
    add_edge(&dag, 1, 2);
    add_edge(&dag, 2, 0);
    add_edge(&dag, 3, 2);

    DFS(dag);

    free_dag(dag);
    return 0;
}
