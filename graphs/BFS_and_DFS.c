#include <stdio.h>
#include <stdlib.h>

struct Node
{
    int value;
    struct Node *next;
};

/**
 * @brief Appends a new node to the TAIL of the list (FIFO behavior)
 */
void enqueue(struct Node **queue, int value)
{
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    if(new_node == NULL) exit(1);
    
    new_node->value = value;
    new_node->next = NULL;

    if(queue[0] == NULL)
    {
        queue[0] = new_node;
        return;
    }

    struct Node *current = queue[0];
    while(current->next != NULL)
    {
        current = current->next;
    }
    current->next = new_node;
}

/**
 * @brief Removes a node from the HEAD of the list (FIFO behavior)
 */
struct Node *dequeue(struct Node **queue)
{
    // Return NULL gracefully so the server doesn't crash!
    if(queue[0] == NULL) return NULL; 

    struct Node *next = queue[0]->next;
    struct Node *head = queue[0];
    queue[0] = next;
    
    head->next = NULL;
    return head;
}

/**
 * @brief Executes Breadth-First Search on an Adjacency Matrix
 */
void BFS(int matrix[7][7], int matrix_size, int start_vertex, struct Node **queue)
{
    int visited[7] = {0}; // 0 means unvisited, 1 means visited
    
    printf("BFS Traversal starting at vertex %d:\n", start_vertex);
    
    // Visit the starting vertex
    visited[start_vertex] = 1;
    printf("%d ", start_vertex);
    enqueue(queue, start_vertex);

    // Loop until the queue is completely empty
    while(queue[0] != NULL)
    {
        // Dequeue exactly ONE vertex to process
        struct Node *current_vertex = dequeue(queue);
        int i = current_vertex->value;
        free(current_vertex); 

        // Check all possible neighbors of vertex 'i'
        for(int j = 1; j < matrix_size; j++)
        {
            // If there is an edge and we haven't visited it yet
            if(matrix[i][j] == 1 && visited[j] == 0)
            {
                printf("%d ", j);      
                visited[j] = 1; 
                enqueue(queue, j);
            }
        }
    }
    printf("\n");
}

/**
 * @brief Executes Depth-First Search on an Adjacency Matrix
 */
void DFS(int matrix[7][7], int matrix_size, int start_vertex)
{
    static int visited[7] = {0};

    if(visited[start_vertex] == 0)
    {
        printf("%d ", start_vertex);
        visited[start_vertex] = 1;
        
        for(int i = 1; i < matrix_size; i++)
        {
            if(matrix[start_vertex][i] == 1 && visited[i] == 0)
            {
                DFS(matrix, matrix_size, i);
            }
        }
    }
    
}

/**
 * @brief Deallocate the nodes stored in the queue 
 */
void free_queue(struct Node **queue)
{
    struct Node *current = queue[0];
    struct Node *next_node = NULL;

    while(current != NULL)
    {
        next_node = current->next;
        free(current);
        current = next_node;
    }

    queue = NULL;
}

int main(void)
{
    // A queue starts as a simple NULL pointer. No ghost nodes!
    struct Node *queue = NULL;

    // Using your exact 1-indexed graph representation
    int matrix[7][7] = {
        {0,0,0,0,0,0,0},
        {0,0,1,1,0,0,0}, // Vertex 1 connects to 2, 3
        {0,1,0,0,1,0,0}, // Vertex 2 connects to 1, 4
        {0,1,0,0,1,0,0}, // Vertex 3 connects to 1, 4
        {0,0,1,1,0,1,1}, // Vertex 4 connects to 2, 3, 5, 6
        {0,0,0,0,1,0,0}, // Vertex 5 connects to 4
        {0,0,0,0,1,0,0}  // Vertex 6 connects to 4
    };

    BFS(matrix, 7, 1, &queue);
    
    // DFS
    printf("DFS Traversal starting at vertex %d:\n", 1);
    DFS(matrix, 7, 1);

    free_queue(&queue);
    return 0;
}
