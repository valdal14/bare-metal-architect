#include <stdio.h>


struct Graph 
{
    int data[5][5];
};

/**
 * @brief Init a new Grahs using adjacent matrix
 * approach. O(n^2)
 * @return struct Graph 
 */
struct Graph init(void)
{
    struct Graph g = {0};
    // represent a Graph with vertexes and edges 
    // using Adjacent Matrix.

    // vertex 0
    g.data[0][1] = 1;
    g.data[0][2] = 1;
    g.data[0][3] = 1;
    // vertex 1
    g.data[1][0] = 1;
    g.data[1][2] = 1;
    // vertex 2
    g.data[2][0] = 1;
    g.data[2][1] = 1;
    g.data[2][3] = 1;
    g.data[2][4] = 1;
    // vertex 3
    g.data[3][0] = 1;
    g.data[3][2] = 1;
    g.data[3][4] = 1;
    // vertex 4 
    g.data[4][3] = 1;
    g.data[4][4] = 1;

    return g;
}

/**
 * @brief Show the edges/links of a given vertex 
 * @param int matrix 
 * @param int vertex
 * @return void
 */
void show_vertex_connection(int matrix[5][5], int vertex)
{
    if(vertex > 4 || vertex < 0) return;

    for(int i = 0; i < 5; i++)
        if(matrix[vertex][i] == 1) 
            printf("Graph vertex %d at index[%d] has edge %d\n", vertex, i, matrix[vertex][i]);
    
    printf("---------------------------------------------\n");
}

/**
 * @brief Prints out a representation of the Graph with vertexes
 * and edges connections
 * @param int matrix
 * @return void
 */
void show_graph(int matrix[5][5])
{
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 5; j++)
        {
            if(matrix[i][j] == 1)
                printf("%d -> %d\n", i, j);
        }
        printf("---------------------\n");
    }
}

int main(void)
{
    struct Graph adj_matrix = init();
    show_vertex_connection(adj_matrix.data, 2);
    show_graph(adj_matrix.data);
    return 0;
}
