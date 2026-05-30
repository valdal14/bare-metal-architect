#ifndef MINHEAP_H
#define MINHEAP_H

#define MINHEAP 0
#define HEAPNODE 1

#include <stddef.h> 

typedef struct HeapNode
{
    int city_id;
    int distance;
} HeapNode;

typedef struct MinHeap
{
    struct HeapNode *heap_nodes;
    int capacity;
    int size;
} MinHeap;

/**
 * @brief Check the allocation of the heap 
 * @param void p pointer
 * @param int type
 */
void check_heap_alloc(void *obj, int type);

/**
 * @brief Init the MinHeap
 * @param MinHeap double pointer
 * @param int capacity
 * @return void
 */
void init_minheap(MinHeap **heap, int capacity);

/**
 * @brief Pushes a new city_id and distance inside the MinHeap
 * @param MinHeap heap pointer
 * @param int city_id
 * @param int distance
 * @return void
 */
void push(MinHeap *heap, int city_id, int distance);

/**
 * @brief Pop an HeapNode from the heap
 * @param MinHeap heap pointer
 * @return HeapNode
 */
struct HeapNode pop(MinHeap *heap);

/**
 * @brief Clean the memory allocated for the MinHeap
 * @param MinHeap heap pointer
 * @return void
 */
void clean_minheap(MinHeap *heap);

#endif
