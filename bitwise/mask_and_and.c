#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define BIT(x) (1 << (x))
#define DOOR 4
#define CMD_CLOSE 0 
#define CMD_OPEN 1

/**
 * @brief Set the state of the door 
 * @param uint8_t home pointer
 * @param uint8_t cmd 
 * @param bool callback 
 * @return bool
 */
bool set_door(uint8_t *home, uint8_t cmd, bool(*callback)(uint8_t *home))
{
    switch(cmd)
    {
        case CMD_OPEN:
            printf("Execute: OPEN DOOR Command\n");
            *home |= BIT(DOOR);
            break;
        case CMD_CLOSE:
            printf("Execute: CLOSE DOOR Command\n");
            *home &= ~BIT(DOOR);
            break;
        default:
            fprintf(stderr, "Invalid command \n");
            return false;
    }

    return callback(home);
}

/**
 * @brief Checks if the door was opened 
 * @param uint8_t home pointer
 * @return bool
 */
bool is_door_open(uint8_t *home)
{
    return (*home & BIT(DOOR));
}

/**
 * @brief Checks if the door was closed 
 * @param uint8_t home pointer
 * @return bool
 */
bool is_door_close(uint8_t *home)
{
    return ((bool)(*home & BIT(DOOR)) == false) ? true : false;
}

/**
 * @brief Assert the state of the door and inform the user
 * @param uint8_t cmd. The executed command
 * @param bool result of the command
 * @return void
 */
void verify_cmd(uint8_t cmd, bool result)
{
   switch(cmd)
    {
        case CMD_OPEN:
            if(result == true) 
                printf("The DOOR is now opened\n");
            else
                fprintf(stderr, "The OPEN command did not work\n");
            break;
        case CMD_CLOSE:
            if(result == true) 
                printf("The DOOR is now closed\n");
            else
                fprintf(stderr, "The CLOSE command did not work\n");
            break;
        default:
            fprintf(stderr, "Invalid command \n");
            return;
    } 
    
}

int main(void)
{
    uint8_t home = 0x00;
    bool open = set_door(&home, CMD_OPEN, is_door_open);
    verify_cmd(CMD_OPEN, open);
    bool close = set_door(&home, CMD_CLOSE, is_door_close);
    verify_cmd(CMD_CLOSE, close);
    return 0;
}
