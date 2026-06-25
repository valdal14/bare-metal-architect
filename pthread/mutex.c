#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// The global mutex variable
pthread_mutex_t mutex;

typedef struct 
{
    int log;
    int is_on;
} Sensor;

void init_sensor(Sensor **sensor)
{
    Sensor *new_sensor = (Sensor *)calloc(1, sizeof(Sensor *));
    if(new_sensor == NULL) exit(EXIT_FAILURE);

    new_sensor->log = 0;
    new_sensor->is_on = 0;

    *sensor = new_sensor;
}

void *toggle_sensor(void *arg)
{
    Sensor *sensor = (Sensor *)arg;
    if(sensor == NULL) exit(EXIT_FAILURE);
    // Apply the LOCK 
    pthread_mutex_lock(&mutex);

    sensor->log += 1;  
    // toggle the switch sensor
    if(sensor->is_on == 0) 
    {
        sensor->is_on = 1;
    }
    else
    { 
        sensor->is_on = 0;
    }
    
    // Release the LOCK
    pthread_mutex_unlock(&mutex);

    return (void *)sensor;
}

void check_sensor_state(Sensor *sensor)
{
    sensor->is_on == 0 ? printf("Sensor OFF\n") : printf("Sensor ON\n");
    printf("Registered toggle operation: %d\n", sensor->log);
}

int main(void)
{
    Sensor *sensor = NULL;
    // declare a new pthread_t variable 
    pthread_t user1;
    // init the global mutex variable 
    pthread_mutex_init(&mutex, NULL);

    init_sensor(&sensor);
    printf("Sensor is now allocated at address %p\n", sensor);

    for(int i = 0; i < 10; i++)
        pthread_create(&user1, NULL, toggle_sensor, (void *)sensor);
    
    for(int i = 0; i < 10; i++)
        pthread_join(user1, (void **)&sensor);

    check_sensor_state(sensor);
    
    return 0;
}
