#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Dijkstra.h"

#define CITIES 6
#define CITY_LEN 10
#define GRAPH 0
#define CITY 1
#define EDGE 2
#define CITY_NAME 3

typedef struct Route
{
    int idx[2];
} Route;

typedef struct EdgeNode
{
    int destination;
    int weight;
    struct EdgeNode *next;
} EdgeNode;

typedef struct City
{
    char *name;
} City;

typedef struct WDG
{
    int num_cities;
    struct City **cities;
    struct EdgeNode **edges;
} WDG;

/**
 * @brief Checks the memory allocation for a given object and type
 * @param void obj pointer
 * @param int type
 * @return void
 */
void check_alloc(void *obj, int type)
{
    switch(type)
    {
        case 0:
            if((WDG *)obj == NULL) 
            {
                fprintf(stderr, "Cannot allocate space for the GRAPH\n");
                exit(1);
            }
            break;
        case 1:
            if((City *)obj == NULL) 
            {
                fprintf(stderr, "Cannot allocate space for the City\n");
                exit(1);       
            }
            break;
        case 2:
            if((EdgeNode *)obj == NULL) 
            {
                fprintf(stderr, "Cannot allocate space for the EdgeNode\n");
                exit(1);
            }
            break;
        case 3:
            if((char *)obj == NULL) 
            {
                fprintf(stderr, "Cannot allocate space for the City's name\n"); 
                exit(1);
            }
            break;
        default:
            fprintf(stderr, "Unsupported type\n");
            exit(1);
    }
}

/**
 * @brief Init the cities
 * @return void
 */
City **init_cities(void)
{
    char city[CITIES][CITY_LEN] = {"Málaga", "Sevilla", "Granada", "Córdoba", "Almería", "Jaén"};
    City **andalucia_cities = (City **)calloc(CITIES, sizeof(City));
    check_alloc(andalucia_cities, CITY);

    for(int i = 0; i < CITIES; i++)
    {
        City *andalucia = (City *)malloc(sizeof(City));
        check_alloc(andalucia, CITY);
        andalucia->name = (char *)malloc(CITY_LEN * sizeof(char));
        check_alloc(andalucia->name, CITY_NAME);
        strncpy(andalucia->name, city[i], (sizeof(char) * CITY_LEN));
        andalucia->name[CITY_LEN - 1] = '\0';
        andalucia_cities[i] = andalucia;
    }

    return andalucia_cities;
}

/**
 * @brief Validates a route 
 * @param WDG graph pointer
 * @param char starting_city pointer
 * @param char destination_city pointer
 * @param int indeces double pointer
 * @return Route
 */
struct Route validate_route(WDG *graph, char *starting_city, char *destination_city)
{
    Route route;
    int starting_idx = -1;
    int destination_idx = -1;
    
    printf("Validating Route:\n");

    for(int i = 0; i < CITIES; i++)
    {
        if(strcmp(starting_city, graph->cities[i]->name) == 0)
        {
            printf("Starting City: %s\n", starting_city);
            starting_idx = i;
        }

        if(strcmp(destination_city, graph->cities[i]->name) == 0)
        {
            printf("Destination City: %s\n", destination_city);
            destination_idx = i;
        }
    }

    if((starting_idx == -1 || destination_idx == -1) || (starting_idx == destination_idx))
    {
        fprintf(stderr, "Invalid route, please check starting and destination city\n");
        exit(1);
    }

    route.idx[0] = starting_idx;
    route.idx[1] = destination_idx;
    return route;
}

/**
 * @brief Adds a new route (edge) from star city to destination city
 * @param WDG graph pointer
 * @param char starting_city pointer
 * @param char destination_city pointer
 * @param int km 
 * @return void
 */
void add_route(WDG *graph, char *starting_city, char *destination_city, int km)
{
    if(km <= 0)
    {
        fprintf(stderr, "Invalid value: The distance cannot be smaller or equal to 0\n");
        exit(1);
    }

    Route route = validate_route(graph, starting_city, destination_city);
    
    EdgeNode *new_route = malloc(sizeof(EdgeNode));
    check_alloc(new_route, EDGE);
    
    // Add the destination city index to the destination
    new_route->destination = route.idx[1];
    new_route->weight = km;

    // Connect the starting_city as the next element in the new_road
    new_route->next = graph->edges[route.idx[0]];

    // add the new_road to the graph
    graph->edges[route.idx[0]] = new_route;
    printf("Route from %s to %s is %d Km\n", graph->cities[route.idx[0]]->name, graph->cities[route.idx[1]]->name, km);
    printf("--------------------------------\n");
}


/**
 * @brief Init a new WDG graph
 * @param WDG wdg double pointer
 * @param City cities double pointer
 * @return void
 */
void init_wdg(WDG **wdg, City **cities)
{
    WDG *graph = (WDG *)calloc(1, sizeof(WDG));
    check_alloc(graph, GRAPH);
    graph->cities = cities;
    EdgeNode **edges = (EdgeNode **)calloc(CITIES, sizeof(EdgeNode *));
    check_alloc(edges, EDGE);
    graph->edges = edges;
    *wdg = graph;
}

/**
 * @brief Clean the allocated memory
 * @param WDG graph pointer
 * @return void
 */
void clean(WDG *graph)
{
    for(int i = 0; i < CITIES; i++)
    {
        EdgeNode *current = graph->edges[i];
        EdgeNode *next_edge = NULL;

        while(current != NULL)
        {
            next_edge = current->next;
            free(current);
            current = next_edge;
        }

        free(graph->cities[i]->name);
        free(graph->cities[i]);
    }
    
    free(graph->edges);
    free(graph->cities);
    free(graph);
    graph = NULL;
}

/**
 * @brief Finds the shortest path using dijkstra's algo
 * @param WDG graph pointer
 * int start_city
 * @return void
 */
void run_dijkstra(WDG *graph, int start_city)
{
    int distances[CITIES];
    
    for (int i = 0; i < CITIES; i++)
    {
        distances[i] = 999999;
    }
    
    distances[start_city] = 0;

    MinHeap *heap = NULL;
    init_minheap(&heap, 20); 
    push(heap, start_city, 0);

    while (heap->size > 0)
    {
        // Extract the Cheapest City
        HeapNode current = pop(heap);
        int current_city = current.city_id;

        // If we found a faster route skip it.
        if (current.distance > distances[current_city]) continue;

        // Traverse the Intersections
        EdgeNode *current_edge = graph->edges[current_city];
        
        while (current_edge != NULL)
        {
            int destination = current_edge->destination;
            int highway_cost = current_edge->weight;

            // Calculate total cost from start to this destination
            int new_distance = distances[current_city] + highway_cost;

            // Check if this is the best route 
            if (new_distance < distances[destination])
            {
                // Update the list  with the faster time
                distances[destination] = new_distance;

                // Push so I can explore outwards from it later
                push(heap, destination, new_distance);
            }

            // Move to the next
            current_edge = current_edge->next;
        }
    }

    printf("\nShortest distances from %s:\n", graph->cities[start_city]->name);
    printf("----------------------------------------\n");
    for (int i = 0; i < CITIES; i++)
    {
        if (distances[i] == 999999)
        {
            printf("To %-10s : UNREACHABLE (No roads available)\n", graph->cities[i]->name);
        }
        else
        {
            printf("To %-10s : %d km\n", graph->cities[i]->name, distances[i]);
        }
    }

    free(heap->heap_nodes);
    free(heap);
}

int main(void)
{
    WDG *wdg = NULL;
    City **cities = init_cities();
    init_wdg(&wdg, cities);
   
    add_route(wdg, "Málaga", "Granada", 125);
    add_route(wdg, "Málaga", "Córdoba", 160);
    add_route(wdg, "Granada", "Jaén", 90);
    add_route(wdg, "Córdoba", "Sevilla", 140);
    
    // 0 is the Málaga idx 
    run_dijkstra(wdg, 0);

    clean(wdg);
	
    return 0;
}
