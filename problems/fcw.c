#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

#define PLANE_DEFAULT_MASK 0x00
#define SWITCH(x) (1 << (x))
// Airplane Controls Masks 
#define GROUND_MASK 0x01 
#define AIRBORN_MASK 0x02
#define FUEl_MASK 0x04
#define GROUND_BIT 0
#define AIRBORNE_BIT 2
// Engine Controls Masks
#define LEFT_ENGINE_MASK 0x02 
#define RIGHT_ENGINE_MASK 0x04 
// Airplane Authorization
#define AUTH_GROUND 0x01
#define AUTH_FUEL 0x02
#define AUTH_L_ENG 0x04
#define AUTH_R_ENG 0x80 
#define AUTH_APPROVAL_CODE 4
// Command Macros
#define MAX_CMD_SIZE 10

typedef enum 
{
    ON,
    OFF,
    IGNORED
} CMDState;

typedef enum 
{
    LEFT,
    RIGHT 
} ControlSwitch;

typedef struct Command 
{
    struct Command *next; // 8 bytes
    char *name; // 8 bytes 
    uint8_t state; // 1 bite
} Command; // 17 + 7 paddings = 24 bytes 

typedef struct
{
    char *id; // 8 byte 
    Command *bb_head; // 8 bytes
    Command *bb_tail; // 8 bytes 
    uint8_t recording; // 1 byte 
    
    /**
     * bit 0 = onground
     * bit 2 = airborn
     * bit 4 = fuel status
     */
    uint8_t flight_controls; // 1 byte 
    /**
     * bit 2 = engine left status
     * bit 4 = engine right status 
     */
    uint8_t engine_control; // 1 byet 
    /**
     * All of the following checks 
     * must be completed to get the 
     * control tower authorization
     *
     * bit 0 = onground
     * bit 2 = fuel status
     * bit 4 = engine left switch  
     * bit 8 = engine right switch
     * */
    uint8_t authorized_to_flight; // 1 byte 
} Airplane; // 28 bytes + 4 paddings = 32 bytes  

/**
 * @brief Records an executed command in the BlackBox
 * @param Airplane airplane pointer 
 * @param chat cmd pointer
 * @param CMDState cmd_state
 * @return void
 */
void record_command(Airplane *airplane, char *cmd, CMDState cmd_state)
{
    Command *command = (Command *)malloc(sizeof(Command));
    
    if(command == NULL)
    {
        fprintf(stderr, "WARNING: '%s' command could not be recorded\n", cmd);
        return;
    }
    
    size_t cmd_size = strlen(cmd) + 1;
    command->name = (char *)malloc(sizeof(char) * cmd_size);
    
    if(command == NULL || cmd_size > MAX_CMD_SIZE)
    {
        fprintf(stderr, "WARNING: '%s' command instruction could not be recorded\n", cmd);
        return;
    }

    strncpy(command->name, cmd, cmd_size);
    command->name[cmd_size - 1] = '\0';
    command->state = cmd_state;
    command->next = NULL;

    if(airplane->bb_head == NULL)
    {
        airplane->bb_head = command;
        airplane->bb_tail = command;
    }
    else
    {
        airplane->bb_tail->next = command;
        airplane->bb_tail = command;
    }
}

/**
 * @brief Prepare the Airplane
 * @param Airplane double pointer
 * @param void rec pointer callback
 * @return void
 */
void init(Airplane **airplane, char *id, void(*rec)(Airplane *airplane, char *cmd, CMDState cmd_state))
{
    Airplane *plane = (Airplane *)calloc(1, sizeof(Airplane));
    
    if(plane == NULL)
    {
        fprintf(stderr, "CODE: Abort Operation\n");
        fprintf(stderr, "Airplane '%s' is cannot be configured for departure\n", id);
        exit(1);
    }

    size_t plane_id_size = strlen(id) + 1;
    plane->id = (char *)malloc(sizeof(char) * plane_id_size);
    
    if(plane->id == NULL)
    {
        fprintf(stderr, "CODE: Abort Operation\n");
        fprintf(stderr, "Airplane without an identifier cannot be configured for departure\n");
        exit(1);
    }

    strncpy(plane->id, id, plane_id_size);
    plane->id[plane_id_size - 1] = '\0';
    plane->flight_controls = PLANE_DEFAULT_MASK;
    plane->engine_control = PLANE_DEFAULT_MASK;
    plane->bb_head = NULL;
    plane->bb_tail = NULL;
    plane->recording = 0;
    // activate the state GROUND 
    plane->flight_controls |= GROUND_MASK;
    CMDState state = ON;
    rec(plane, "IS-GROUND", state);
    // Authorization GROUD granted
    plane->authorized_to_flight |= AUTH_GROUND;

    *airplane = plane;
}

/**
 * @brief Maps the CMDState state value to its string representation
 * @param CMDState cmd_state
 * @return char pointer
 */
char *cmd_state_to_str(CMDState cmd_state)
{
    char *str = (char *)malloc(sizeof(char) * MAX_CMD_SIZE);

    switch(cmd_state)
    {
        case ON:
            str = "ON";
            break;
        case OFF:
            str = "OFF";
            break;
        case IGNORED:
            str = "IGNORED";
            break;
        default:
            fprintf(stderr, "ALERT: Invalid CMDState found, departure aborted\n");
            exit(1);
    }

    size_t cmd_size = strlen(str);
    str[cmd_size - 1] = '\n';

    return str;
}

/**
 * @brief Refills the Airplane's tanks
 * @param Airplane airplane pointer
 * @param void rec pointer callback
 * @return void
 */
void refill(Airplane *airplane, void(*rec)(Airplane *airplane, char *cmd, CMDState cmd_state))
{
    if((airplane->flight_controls & GROUND_MASK) == 0)
    {
        fprintf(stderr, "Flight %s GROUND control state is NOT valid. Operation aborted\n", airplane->id);
        exit(1);
    }

    if((airplane->flight_controls & FUEl_MASK) == 0 && (airplane->flight_controls & GROUND_MASK) != 0)
    {
        printf("EXEC: Re-filling Airplane's Tanks\n");
        airplane->flight_controls |= FUEl_MASK;
        airplane->authorized_to_flight |= AUTH_FUEL;
        CMDState state = ON;
        rec(airplane, "REFILL", state); 
    }
    else
    {
        printf("The Airplane's Tanks are already filled\n");
        CMDState state = IGNORED;
        rec(airplane, "REFILL", state);
    }
}

/**
 * @brief Toggles the Engine Control Switch 
 * @param Airplane airplane pointer
 * @param ControlSwitch cs 
 * @param void rec pointer callback
 * @return void
 */
void toggle_engine_switch_on(Airplane *airplane, ControlSwitch cs, void(*rec)(Airplane *airplane, char *cmd, CMDState cmd_state))
{
    if((airplane->engine_control & LEFT_ENGINE_MASK) != 0 && (airplane->engine_control & RIGHT_ENGINE_MASK) != 0)
    {
        printf("WARNING: Both Left and Right engines are already ON\n");
        CMDState state = IGNORED;
        rec(airplane, "ENGINE-ON", state);
        return;
    }

    switch(cs)
    {
        case LEFT:
        {
            if((airplane->engine_control & LEFT_ENGINE_MASK) == 0 && (airplane->flight_controls & GROUND_MASK) != 0)
            {
                printf("EXEC: Toggle ON Left Control Engine Switch\n");
                airplane->engine_control |= LEFT_ENGINE_MASK;
                airplane->authorized_to_flight |= AUTH_L_ENG;
                CMDState state = ON;
                rec(airplane, "LEFT-ENG", state);
            }
            else
            {
                fprintf(stderr, "ALERT: An attempt to turn the LEFT engine ON failed. Departure aborted\n");
                CMDState state = IGNORED;
                rec(airplane, "LEFT-ENG", state);
                exit(1);
            }
            break;
        }
        case RIGHT:
        {
            if((airplane->engine_control & RIGHT_ENGINE_MASK) == 0 && (airplane->flight_controls & GROUND_MASK) != 0)
            {
                printf("EXEC: Toggle ON Right Control Engine Switch\n");
                airplane->engine_control |= RIGHT_ENGINE_MASK;
                airplane->authorized_to_flight |= AUTH_R_ENG;
                CMDState state = ON;
                rec(airplane, "RIGHT-ENG", state);
            }
            else 
            {
                fprintf(stderr, "ALERT: An attempt to turn the RIGHT engine ON failed. Departure aborted\n");
                CMDState state = IGNORED;
                rec(airplane, "RIGHT-ENG", state);
                exit(1);
            }
            break;
        }
        default:
            fprintf(stderr, "CODE: Invalid ControlSwitch, the Operation will be aborted\n");
            CMDState state = IGNORED;
            rec(airplane, "ENGINE-ON", state);
            exit(1);
    }
}

/**
 * @brief Request the authorization to flight after
 * completion of the departure checklist 
 * @param Airplane airplane pointer
 * @return void
 */
void request_checklist_auth(Airplane *airplane)
{
    uint8_t approval_steps = 0;
    uint8_t checklist_status = airplane->authorized_to_flight;
    if((checklist_status & AUTH_GROUND) != 0) approval_steps += 1;
    if((checklist_status & AUTH_FUEL) != 0) approval_steps += 1;
    if((checklist_status & AUTH_L_ENG) != 0) approval_steps += 1;
    if((checklist_status & AUTH_R_ENG) != 0) approval_steps += 1;
    
    if(approval_steps == AUTH_APPROVAL_CODE)
    {
        printf("Flight %s is authorized for departure\n", airplane->id);
    }
    else
    {
        printf("STATUS = %d\n", approval_steps);
        fprintf(stderr, "Flight %s authorization DENIED\n", airplane->id);
        exit(1);
    }
}

/**
 * @brief Show the Fligh Recording Data 
 * @param Airplane airplane pointer
 * @return void
 */
void show_flight_recordings(Airplane *airplane)
{
    Command *head = airplane->bb_head;
    printf("\n------ FLIGHT RECORDING DATA ------\n");

    while(head != NULL)
    {
        printf("COMMAND  : %s\n", head->name);
        printf("CMD STATE: %s\n", cmd_state_to_str(head->state));
        printf("-----------------------------------\n");
        head = head->next;
    }
}

/**
 * @brief Executes the departure
 * @param Airplane airplane pointer
 * @param void rec pointer callback
 * @return void
 */
void execute_departure(Airplane *airplane, void(*rec)(Airplane *airplane, char *cmd, CMDState cmd_state))
{
    request_checklist_auth(airplane); 
    printf("Flight %s is leaving the runaway\n", airplane->id);
    // Toggle the GROUND switch OFF 
    airplane->flight_controls &= ~SWITCH(GROUND_BIT);
    
    if((airplane->flight_controls & GROUND_MASK) == 0)
    {
        CMDState state = OFF;
        rec(airplane, "IS-GROUND", state);
    }
    else
    {
        fprintf(stderr, "ALERT: GROUND Switch failed, departure aborted\n");
        exit(1);
    }
    
    // Set the Airplane to AIRBORN state
    airplane->flight_controls |= SWITCH(AIRBORNE_BIT);
    CMDState state = ON;
    rec(airplane, "IS-ONAIR", state);
    printf("Flight %s is successfully AIRBORN\n", airplane->id);
}

int main(void)
{
    Airplane *plane = NULL;
    init(&plane, "WA-777", record_command);
    refill(plane, record_command);
    toggle_engine_switch_on(plane, RIGHT, record_command);
    toggle_engine_switch_on(plane, LEFT, record_command);
    toggle_engine_switch_on(plane, LEFT, record_command);
    execute_departure(plane, record_command);
    
    show_flight_recordings(plane);
    return 0;
}
