#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CITIES 6
#define CITY_LEN 10
#define GRAPH 0
#define CITY 1
#define EDGE 2
#define CITY_NAME 3

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
            }
            return;
        case 1:
            if((City *)obj == NULL)
            {
                fprintf(stderr, "Cannot allocate space for the City\n");
            }
            return;
        case 2:
            if((EdgeNode *)obj == NULL)
            {
                fprintf(stderr, "Cannot allocate space for the EdgeNode\n");
            }
            return;
        case 3:
            if((char *)obj == NULL)
            {
                fprintf(stderr, "Cannot allocate space for the City's name\n"); 
            }
            return;
        default:
            fprintf(stderr, "Unsupported type\n");
            break;
    }

    exit(1);
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
        City *andalucia = (City *)malloc(sizeof(city));
        check_alloc(andalucia, CITY);
        andalucia->name = (char *)malloc(CITY_LEN * sizeof(char));
        check_alloc(andalucia->name, CITY_NAME);
        strncpy(andalucia->name, city[i], (sizeof(char) * CITY_LEN));
        andalucia->name[CITY_LEN] = '\0';
        andalucia_cities[i] = andalucia;
    }

    return andalucia_cities;
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
    *wdg = graph;
}

int main(void)
{
    WDG *wdg = NULL;
    City **cities = init_cities();
    init_wdg(&wdg, cities);
    for(int i = 0; i < CITIES; i++)
        printf("%s\n", wdg->cities[i]->name);

    return 0;
}
