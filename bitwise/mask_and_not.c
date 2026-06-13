#include <stdio.h>
#include <stdint.h>

#define BIT(x) (1 << (x))
#define HEATER 3
#define ALARM 7

/**
 * Executes the routine for leaving the house by activating the heater and alarm.
 * Sets the corresponding bits in the home state variable and triggers a callback.
 *
 * @param home     Pointer to the uint8_t variable representing the home state.
 * @param callback Function pointer to a routine verification function.
 * @return void
 */
void exec_routine_leave_house(uint8_t *home, void(*callback)(uint8_t *home))
{
    uint8_t set_routine = BIT(HEATER) | BIT(ALARM);
    // set the new bits but do not touch existing turned on bit if there are
    *home |= set_routine;
    callback(home);
}

/**
 * Verifies that the leave house routine was executed correctly.
 * Checks if both the HEATER and ALARM bits are set in the home state.
 * Prints a success or failure message to standard output.
 *
 * @param home Pointer to the uint8_t variable representing the home state.
 * @return void
 */
void verify_routine(uint8_t *home)
{
    uint8_t routine_count = 0;

    if((*home & BIT(HEATER)) != 0) routine_count++;
    if((*home & BIT(ALARM)) != 0) routine_count++;

    if(routine_count == 2)
    {
        printf("Command executed successfully, both devices are ON\n");
    }
    else 
    {
        printf("Something went wrong, please try again\n");
    }
}

int main(void)
{
    uint8_t home = 0x00;
    exec_routine_leave_house(&home, verify_routine);
    return 0;
}
