#include <stdio.h>
#include <stdlib.h>
#include "Dijkstra.h"

/**
 * @brief Check the allocation of the heap 
 * @param void obj pointer
 * @param int type
 */
void check_heap_alloc(void *obj, int type)
{
    switch(type)
    {
        case 0:
            if((MinHeap *)obj == NULL)
            {
                fprintf(stderr, "Cannot allocate space for the MinHeap\n");
                exit(1);
            }
            break;
        case 1:
            if((HeapNode *)obj == NULL)
            {
                fprintf(stderr, "Cannot allocate space for the HeapNode\n");
                exit(1);
            }
            break;
        default:
            fprintf(stderr, "Unsupported type\n");
            exit(1);
    }
}

/**
 * @brief Init MinHeap
 * @param MinHeap double pointer
 * @param int capacity
 * @return void
 */
void init_minheap(MinHeap **heap, int capacity)
{
    MinHeap *min_heap = (MinHeap *)calloc(1, sizeof(MinHeap));
    check_heap_alloc(min_heap, MINHEAP);
    min_heap->capacity = capacity;
    min_heap->size = 0;

    HeapNode *heap_nodes = (HeapNode *)calloc(min_heap->capacity, sizeof(HeapNode));
    check_heap_alloc(heap_nodes, HEAPNODE);

    min_heap->heap_nodes = heap_nodes;
    *heap = min_heap;
}

/**
 * @brief Pushes a new city_id and distance inside the MinHeap
 * @param MinHeap heap pointer
 * @param int city_id
 * @param int distance
 * @return void
 */
void push(MinHeap *heap, int city_id, int distance)
{
    if(heap->size + 1 > heap->capacity)
    {
        fprintf(stderr, "Max heap capacity reached, the program will terminate\n");
        exit(1);
    }

	HeapNode node;
	node.city_id = city_id;
	node.distance = distance;
    
	int current = heap->size;
    
	heap->heap_nodes[current] = node;
    
	heap->size += 1;
    
	while(current > 0)
	{
	    int parent = (current - 1) / 2;
        
	    if(heap->heap_nodes[parent].distance <= heap->heap_nodes[current].distance) break;
        
	    // Swap if the parent is strictly greater than the current
	    HeapNode temp = heap->heap_nodes[current];
	    heap->heap_nodes[current] = heap->heap_nodes[parent];
	    heap->heap_nodes[parent] = temp;
        
	    // Move focus up to the parent's slot
	    current = parent;
	}
}

/**
 * @brief Pops the smallest node (root) from the MinHeap and fixes the tree
 * @param MinHeap heap pointer
 * @return HeapNode
 */
HeapNode pop(MinHeap *heap)
{
    // Safety check: Cannot extract from an empty heap
    if(heap->size <= 0)
    {
        fprintf(stderr, "Error: Heap is empty, cannot pop\n");
        exit(1);
    }

    if(heap->size == 1)
    {
        heap->size--;
        return heap->heap_nodes[0];
    }

    // Save the absolute shortest distance to return later
    HeapNode root = heap->heap_nodes[0];

    // Move the very last leaf to the top of the tree
    heap->heap_nodes[0] = heap->heap_nodes[heap->size - 1];
    heap->size--;

    int current = 0;

    // Keep looping as long as we have at least one child
    while((2 * current) + 1 < heap->size)
    {
        int left_child = (2 * current) + 1;
        int right_child = (2 * current) + 2;
        // Always assume left is smallest to start
        int smallest = left_child; 

        // Check if the right child actually exists AND if it is cheaper than the left child
        if(right_child < heap->size && heap->heap_nodes[right_child].distance < heap->heap_nodes[left_child].distance)
        {
            smallest = right_child;
        }

        // Compare the parent (current) with the absolute smallest child
        if(heap->heap_nodes[current].distance > heap->heap_nodes[smallest].distance)
        {
            // Swap the parent with the smallest child
            HeapNode temp = heap->heap_nodes[current];
            heap->heap_nodes[current] = heap->heap_nodes[smallest];
            heap->heap_nodes[smallest] = temp;

            // Move the focus down the tree to the new position to check the next row
            current = smallest;
        }
        else
        {
            // The parent is perfectly placed (smaller than both children).
            break;
        }
    }

    // Return the original shortest distance
    return root;
}
