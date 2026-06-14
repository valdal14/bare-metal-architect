#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define BIT(x) (1 << (x))
#define MASK_ON 0x80
#define MASK_OFF 0x00 
#define MASK_FAN_ISOLATE 0x70
#define FAN_SHIFT_OFFSET 4
#define MASK_FAN_CLEAR 0x8F

#define TURN_ON_VAL 128 
#define MAX_SPEED_VAL 7
#define MIN_SPEED_VAL 0 

#define INCREASE 1
#define DECREASE 0

#define BREAK printf("---------------------------------------------\n");
/*
Bit 0, 1, 2: Controls the LED color.
Bit 3: Controls whether the fan is oscillating (True/False).
Bit 4, 5, 6: Controls the fan speed (0 to 7).
Bit 7: Master Power Switch (ON/OFF).

Controlling the Speed 

000 = Speed 0
001 = Speed 1
010 = Speed 2
011 = Speed 3
100 = Speed 4
101 = Speed 5
110 = Speed 6
111 = Speed 7
*/

/**
 * @brief Checks if the fan is turned on 
 * @param fan Pointer to the uint8_t configuration register.
 * @return uint8_t The isolated value of the ON bit (128 if ON, 0 if OFF).
 */
uint8_t is_on(uint8_t *fan)
{
    return *fan & MASK_ON;
}

/**
 * @brief Extracts the current fan speed from the hardware configuration register.
 * @param fan Pointer to the uint8_t configuration register. 
 * @return uint8_t The value of the current speed (0 to 7).
 */
uint8_t get_fan_speed(uint8_t *fan)
{
    // Check the first from the right 4 bits excluding the ON 
    // and returns the value of the current speed by shifting 
    // right using the FAN_SHIFT_OFFSET of 4 bits
    return (*fan & MASK_FAN_ISOLATE) >> FAN_SHIFT_OFFSET;
}

/**
 * @brief Sets the fan speed directly using bitwise clearing and shifting.
 * @param fan Pointer to the uint8_t configuration register.
 * @param speed The target speed value (0 to 7).
 * @return void
 */
void set_speed_direct(uint8_t *fan, uint8_t speed)
{
    // Clear the zone (*fan & MASK_FAN_CLEAR)
    // Shift the integer left by 4 (speed << FAN_SHIFT_OFFSET)
    // Drop it in using OR (|)
    *fan = (*fan & MASK_FAN_CLEAR) | (speed << FAN_SHIFT_OFFSET);
}

/**
 * @brief Reset the fan speed state (min speed) without affecting other hardware bits.
 * @param fan Pointer to the uint8_t configuration register.
 * @return void
 */
void reset_speed_flags(uint8_t *fan)
{
    *fan = (*fan & MASK_FAN_CLEAR) | 0;

    if(get_fan_speed(fan) != 0)
    {
        fprintf(stderr, "Reset failed, turning the fan off...\n");
        exit(1);    
    }
}

/**
 * @brief Checks the status of a given command to ensure state was applied correctly.
 * @param fan Pointer to the uint8_t configuration register.
 * @param value The expected speed value.
 * @return void
 */
void check_speed_status(uint8_t *fan, uint8_t value)
{
    if(get_fan_speed(fan) != value)
    {
        fprintf(stderr, "Error setting speed, resetting the fan, please wait\n");
        reset_speed_flags(fan);
    }
    else
    {
        printf("CMD Executed: Fan spinning speed set correctly\n");
        BREAK;
    }
}

/**
 * @brief Sets the fan directly to maximum speed.  
 * @param fan Pointer to the uint8_t configuration register.
 * @return void
 */
void set_max_speed(uint8_t *fan)
{ 
    if(is_on(fan) == TURN_ON_VAL)
    {
        printf("EXEC CMD: Setting max speed\n");
        set_speed_direct(fan, MAX_SPEED_VAL);
        check_speed_status(fan, MAX_SPEED_VAL);
    }
    else
    {
        fprintf(stderr, "Cannot set Max speed when the fan is OFF\n");
        exit(1);
    }
}

/**
 * @brief Sets the fan to minimum speed by triggering a reset.
 * @param fan Pointer to the uint8_t configuration register.
 * @return void
 */
void set_min_speed(uint8_t *fan)
{
    printf("EXEC CMD: Setting MIN speed\n");
    reset_speed_flags(fan);
    check_speed_status(fan, MIN_SPEED_VAL);
}

/**
 * @brief Turns the fan on without overwriting existing speed or LED states.
 * @param fan Pointer to the uint8_t configuration register.
 * @return void
 */
void turn_on(uint8_t *fan)
{
    if(is_on(fan) == TURN_ON_VAL) return;

    printf("EXEC CMD: Turning the fan ON\n");
    *fan |= BIT(7);

    if(is_on(fan) == TURN_ON_VAL)
    {
        printf("Fan is ON and spinning with default values\n");
        BREAK;
    }
    else
    {
        fprintf(stderr, "Something went wrong, could not turn the fan ON\n");
        exit(1);
    }
}

/**
 * @brief Safely turns off the fan power switch without wiping speed, LED, or oscillator memory.
 * @param fan Pointer to the uint8_t configuration register.
 * @return void
 */
void turn_off(uint8_t *fan)
{
    printf("EXEC CMD: Turning the fan OFF\n");
    *fan &= ~BIT(7); 
}

/**
 * @brief Increase or decrease the speed fan by 1 level using raw integers.
 * @param fan Pointer to the uint8_t configuration register.
 * @param cmd The command to execute (INCREASE or DECREASE).
 * @return void
 */
void change_speed(uint8_t *fan, uint8_t cmd)
{
    if(is_on(fan) == TURN_ON_VAL)
    {
        uint8_t current_speed = get_fan_speed(fan);
        
        switch(cmd)
        {
            case INCREASE:
                if(current_speed == MAX_SPEED_VAL)
                {
                    fprintf(stderr, "Fan is already running at MAX speed\n");
                    return;
                }
                printf("EXEC CMD: Increase speed fan by 1 level\n");
                set_speed_direct(fan, current_speed + 1);
                printf("Fan Current Speed: %d\n", get_fan_speed(fan));
                BREAK;
                break;

            case DECREASE:
                if(current_speed == MIN_SPEED_VAL)
                {
                    fprintf(stderr, "Fan is already running at MIN speed\n");
                    return;
                }
                printf("EXEC CMD: Decrease speed fan by 1 level\n");
                set_speed_direct(fan, current_speed - 1);
                printf("Fan Current Speed: %d\n", get_fan_speed(fan));
                BREAK;
                break;
                
            default:
                return;
        }
    }
}

int main(void)
{
    uint8_t fan_config = MASK_OFF; 
    turn_on(&fan_config); 
    set_max_speed(&fan_config);
    set_min_speed(&fan_config);
    change_speed(&fan_config, INCREASE);
    change_speed(&fan_config, INCREASE);
    change_speed(&fan_config, INCREASE);
    change_speed(&fan_config, INCREASE);
    change_speed(&fan_config, DECREASE);
    change_speed(&fan_config, DECREASE);
    turn_off(&fan_config);
    return 0;
}
