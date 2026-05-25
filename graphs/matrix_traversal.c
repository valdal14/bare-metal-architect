#include <stdio.h>

#define MATRIX_W 5
#define MATRIX_H 4

void print(int matrix[MATRIX_H][MATRIX_W])
{ 
    for(int h = 0; h < MATRIX_H; h++)
    {
        printf("MATRIX[%d] = [ ", h);
        
        for(int w = 0; w < MATRIX_W; w++)
        {
            printf("%d ", matrix[h][w]);
        }

        printf("]\n");
    }

    printf("-------------------------\n");
}

void dfs_probe(int matrix[MATRIX_H][MATRIX_W], int h, int w)
{
    if (h < 0 || h >= MATRIX_H || w < 0 || w >= MATRIX_W) return;

    if (matrix[h][w] == 0) return;

    matrix[h][w] = 0;

    dfs_probe(matrix, h - 1, w); 
    dfs_probe(matrix, h + 1, w);
    dfs_probe(matrix, h, w - 1);
    dfs_probe(matrix, h, w + 1);
}

int numIslands(int matrix[MATRIX_H][MATRIX_W]) 
{ 
    int islands = 0;

    for(int h = 0; h < MATRIX_H; h++)
    {
        for(int w = 0; w < MATRIX_W; w++)
        {
            if(matrix[h][w] == 1)
            {
                islands++;
                dfs_probe(matrix, h, w);
            }
        }
    }
    
    return islands;
}

void fill_the_islands(int matrix[MATRIX_H][MATRIX_W])
{
    matrix[0][0] = 1;
    matrix[0][1] = 1;
    matrix[1][0] = 1;
    matrix[1][1] = 1;
    matrix[2][2] = 1;
    matrix[3][3] = 1;
    matrix[3][4] = 1;
}

int main(void)
{
    int matrix[MATRIX_H][MATRIX_W] = {0};
    fill_the_islands(matrix);
    print(matrix);
    
    int islands = numIslands(matrix);
    print(matrix); 
    printf("Found %d islands\n", islands);
    return 0;
}
