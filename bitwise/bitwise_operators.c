#include <stdio.h>
#include <stdint.h> // Required for uint8_t

/**
 * @brief Turns on the 'Power' bit (Bit 0) without affecting other bits.
 * @param status Pointer to the 8-bit status register.
 * @return void
 */
void turn_on_power(uint8_t *status)
{
    *status = *status | (1 << 0);
}

/**
 * @brief Checks if the 'Network' bit (Bit 1) is active.
 * @param status The 8-bit status register (passed by value is fine here).
 * @return int 1 if connected, 0 if disconnected.
 */
int is_connected(uint8_t status)
{
    return status & (1 << 1) ? 1 : 0;
}

/**
 * @brief Flips the 'Error' bit (Bit 2). If it's 1, make it 0. If 0, make it 1.
 * @param status Pointer to the 8-bit status register.
 * @return void
 */
void toggle_error(uint8_t *status)
{
    *status = *status ^ (1 << 2);
}

int main(void)
{
    // Start with all bits at 0 (0000 0000)
    uint8_t hardware_status = 0; 
    
    turn_on_power(&hardware_status);
    // Should print 1 (0000 0001)
    printf("Status after power on: %d\n", hardware_status); 

    int connected = is_connected(hardware_status);
    // Should print 0
    printf("Is connected? %d\n", connected); 

    toggle_error(&hardware_status);
    // Power is 1, Error is 4. Total should be 5 (0000 0101)
    printf("Status after toggling error: %d\n", hardware_status); 

    return 0;
}
