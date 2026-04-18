#include <stdio.h>
#include <stdlib.h>

#define INT 0
#define CHAR 1
#define STRUCT 2

struct SensorData
{
    int id;
    float temp;
};

/**
 * @brief Interprets and prints generic memory based on a provided type code.
 * @param data A void pointer to the raw data in memory.
 * @param type_code An integer representing the data type (1 = int, 2 = char, 3 = SensorData).
 * @return void
 */
void print_generic(void *data, int type_code)
{
    struct SensorData sensor;

    switch (type_code)
    {
        case INT:
            printf("Integer value = %d\n", *(int *)data);
            break;
        case CHAR:
            printf("Char data = %c\n", *(char *)data);
            break;
        case STRUCT:
            sensor = *(struct SensorData *)data;
            printf("Sensor Id = %d\n",sensor.id);
            printf("Sensor Temp = %f\n",sensor.temp);
            break;
        default:
            fprintf(stderr, "Unsupported data type\n");
            break;
    }
}

/**
 * @brief Allocate, assign and returns a new SensorData pointer
 * @param id An integer value that represent the id of the sensor
 * @param temp A float value that represent the temperature of the sensor
 * @return SensorData pointer
 * 
 */
struct SensorData* init_sensor(int id, float temp)
{
    struct SensorData *data = (struct SensorData *)malloc(sizeof(struct SensorData));
    if(data == NULL)
    {
        fprintf(stderr, "Error allocating space for the sensor\n");
        exit(1);
    }
    
    data->id = id;
    data->temp = temp;
    return data;
}

int main(void)
{
    int num = 14;
    char c = 'c';
    struct SensorData *sensor = init_sensor(2014, 27.5);

    print_generic(&num, INT);
    print_generic(&c, CHAR);
    print_generic(sensor, STRUCT);
    free(sensor);

    return 0;
}
