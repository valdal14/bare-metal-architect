#include <stdio.h>

struct Engine
{
    int hp;
};

struct Car
{
    struct Engine *motor;
};

/**
 * @brief Tune the car engine
 * @param struct Car pointer
 * @return void
 */
void tune_car(struct Car *car)
{
    // Since car is a pointer to get to the value
    // we use the -> chain. -> dereferece + access
    printf("Current power = %d\n",car->motor->hp);
    printf("Tuning the engine...\n");
    car->motor->hp = car->motor->hp + 50;
}

int main(void)
{
    struct Engine engine;
    engine.hp = 400;

    struct Car car;
    // car->motor is a pointer, and we must follow the same rules 
    // for pointers of the same type:
    // I.E int a = 4; int *p_a = &a; 
    // We  assing the pointer the memory address of 'a' 
    // to the value stored in memory of the variable.
    car.motor = &engine;

    tune_car(&car);
    printf("Engine tuned to %d\n", car.motor->hp);
    return 0;
}
