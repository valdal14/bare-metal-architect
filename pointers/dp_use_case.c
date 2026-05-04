#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STATION_SIZE 64
#define C 0
#define F 1

struct Weather
{
    char *station;
    double temp;
    char degree; 
};

/**
 * @brief Adds an entry to the Weather struct, allocating memory for the station name.
 *        Overrides it if we enter a new entry.
 * @param p Double pointer to the Weather struct to be populated.
 * @param s_name The name of the weather station.
 * @param temp The temperature value to record.
 * @return void
 */
void add_entry(struct Weather **p, char *s_name, double temp)
{
    // We only allocate if it hasn't been allocated yet to avoid leaks!
    if(p[0]->station == NULL) {
        p[0]->station = (char *)malloc(STATION_SIZE);
    }
    strncpy(p[0]->station, s_name, STATION_SIZE);
    p[0]->station[STATION_SIZE - 1] = '\0';
    p[0]->degree = 'C';
    p[0]->temp = temp;    
}

/**
 * @brief Converts the temperature from C to F and vice versa based on the given type.
 * @param p Double pointer to the Weather struct containing the temperature.
 * @param type Integer macro representing the target temperature scale (C or F).
 * @return void
 */
void convert(struct Weather **p, int type)
{
    switch(type)
    {
        case C:
        {
            double cel = (p[0]->temp - 32) / 1.8;
            p[0]->temp = cel;
            p[0]->degree = 'C'; 
            break;
        }
        case F:
        {
            double far = (p[0]->temp * 1.8) + 32; 
            p[0]->temp = far;
            p[0]->degree = 'F'; 
            break;
        }
        default:
            fprintf(stderr, "[ERROR] Type is not supported\n");
            exit(1);
    }
}

/**
 * @brief Builds a formatted report by safely copying data from the backend to the channel struct.
 *        Prepends "Estación " or "Station " based on the current temperature degree.
 * @param backend Pointer to the raw backend Weather struct.
 * @param channel Pointer to the presentation Weather struct to be populated.
 * @return void
 */
void build_report(struct Weather *backend, struct Weather *channel)
{
    if(channel->station == NULL) {
        channel->station = (char *)malloc(STATION_SIZE);
    }
    
    // Safely write directly into the channel's heap memory
    if(backend->degree == 'C')
    {
        snprintf(channel->station, STATION_SIZE, "Estación %s", backend->station);
    }
    else
    {
        snprintf(channel->station, STATION_SIZE, "Station %s", backend->station);
    }
    
    // Copy the raw data values over
    channel->temp = backend->temp;
    channel->degree = backend->degree;
}

/**
 * @brief Simulates the transfer of the formatted weather message to standard output.
 * @param p Pointer to the Weather struct to be printed.
 * @return void
 */
void send_to_channel(struct Weather *p)
{
    if(p->degree == 'C')
    {
        printf("%s\n", p->station);
        printf("Temperatura = %.2f\n", p->temp);
        printf("Grados %c.\n", p->degree);
        printf("--------------------------------\n");
    }
    else 
    {
        printf("%s\n", p->station);
        printf("Temperature = %.2f\n", p->temp);
        printf("Degrees %c.\n", p->degree);
        printf("--------------------------------\n");
    }
}

/**
 * @brief Safely frees all dynamically allocated memory inside a Weather struct, 
 *        then frees the struct itself.
 * @param p Pointer to the Weather struct to be destroyed.
 * @return void
 */
void free_ram(struct Weather *p)
{
    if (p != NULL) {
        if (p->station != NULL) {
            free(p->station);
        }
        free(p);
    }
}

int main(void)
{
    // 1. Setup Backend
    struct Weather *weather_backend = (struct Weather *)calloc(1, sizeof(struct Weather));
    if(weather_backend == NULL) exit(1);

    add_entry(&weather_backend, "Malagueta", 24.6);
    convert(&weather_backend, F);   
    
    // 2. Setup Channel
    struct Weather *wc = (struct Weather *)calloc(1, sizeof(struct Weather));
    if(wc == NULL) exit(1);

    // 3. Process
    build_report(weather_backend, wc);
    send_to_channel(wc);
    
    // 4. Cleanup 
    free_ram(weather_backend);
    free_ram(wc);
    
    return 0;
}
