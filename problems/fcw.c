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
#define LANDING_GEAR_MASK 0x80 
#define GROUND_BIT 0
#define AIRBORNE_BIT 1
#define LANDING_GEAR_BIT 7 
// Engine Controls Masks
#define LEFT_ENGINE_MASK 0x02 
#define RIGHT_ENGINE_MASK 0x04 
#define LEFT_ENGINE_BIT 1 
#define RIGHT_ENGINE_BIT 3
// Airplane Authorization
#define AUTH_GROUND 0x01
#define AUTH_FUEL 0x02
#define AUTH_L_ENG 0x04
#define AUTH_R_ENG 0x80 
#define AUTH_APPROVAL_CODE 4
// Command Macros
#define MAX_CMD_SIZE 10
// Pilots Macros
#define P1 "P-01"
#define P2 "P-02"

typedef enum 
{
    ON,
    OFF,
    IGNORED,
    MAYDAY
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
     * bit 8 = landing gear
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
        case MAYDAY:
            str = "MAYDAY";
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
 * @brief Toggles the Engine Control Switch ON  
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
void request_departure_auth(Airplane *airplane)
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
    request_departure_auth(airplane); 
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
    
    // Gear up 
    airplane->flight_controls |= LANDING_GEAR_MASK;
    CMDState lg_state = OFF;
    rec(airplane, "LAND-GEAR", lg_state);
    // Set the Airplane to AIRBORN state
    airplane->flight_controls |= SWITCH(AIRBORNE_BIT);
    CMDState state = ON;
    rec(airplane, "IS-ONAIR", state);
}

/**
 * @brief Allows the pilots to declare and emergency
 * @param Airplane airplane pointer
 * @param char pilot_id pointer
 * @return uint8_t
 */
uint8_t declare_emergency(Airplane *airplane, char *pilot_id)
{
    char buffer[256];
    printf("Flight %s EMERGENCY declaration process\n", airplane->id);
    printf("%s please confirm emergency and engine off authorization\n", pilot_id);

    while(1) 
    {
        printf("CT> ");

        if(fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = '\0';
            
            if(strcmp(buffer, "YES") == 0 || strcmp(buffer, "yes") == 0)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
}

/**
 * @brief Switch Off the Airplane's Engine
 * @param Airplane airplane pointer
 * @param ControlSwitch cs
 * @param void rec pointer callback
 * @return void
 */
void turn_eng_off(Airplane *airplane, ControlSwitch cs, void(*rec)(Airplane *airplane, char *cmd, CMDState cmd_state))
{
    switch(cs)
    {
        case LEFT:
            airplane->engine_control &= ~SWITCH(LEFT_ENGINE_BIT);
            CMDState left_eng_off = ON;
            rec(airplane, "L-ENG-OFF", left_eng_off);
            printf("Flight %s. LEFT engine is now OFF\n", airplane->id);
            break;
        case RIGHT:
            airplane->engine_control &= ~SWITCH(RIGHT_ENGINE_BIT);
            CMDState right_eng_off = ON;
            rec(airplane, "R-ENG-OFF", right_eng_off);
            printf("Flight %s. RIGHT engine is now OFF\n", airplane->id);
            break;
        default:
            fprintf(stderr, "CODE: Invalid ControlSwitch, the Operation will be aborted\n");
            CMDState state = IGNORED;
            rec(airplane, "ENG-OFF", state);
            exit(1);
    }
}

/**
 * @brief Toggles the Engine Control Switch OFF
 * @param Airplane airplane pointer
 * @param ControlSwitch cs 
 * @param void rec pointer callback
 * @return void
 */
void toggle_engine_switch_off(Airplane *airplane, ControlSwitch cs, void(*rec)(Airplane *airplane, char *cmd, CMDState cmd_state))
{
    if((airplane->flight_controls & AIRBORN_MASK) != 0)
    {
        fprintf(stderr, "Flight %s is AIRBORN, an attempt to turn an ENGINE OFF could be catastrophic.\n", airplane->id);
        CMDState state = IGNORED;
        rec(airplane, "ABENG-OFF", state);
        fprintf(stderr, "Flight %s, if this is an emergency request please ask Control Tower Approval\n", airplane->id);
        uint8_t p1_code = declare_emergency(airplane, P1);
        uint8_t p2_code = declare_emergency(airplane, P2);

        if(p1_code == 1 && p2_code == 1)
        {
            fprintf(stderr, "EMERGENCY Confirmed. You are authorized to switch the engine OFF\n");
            CMDState emergency = ON;
            rec(airplane, "EMERGENCY", emergency);
            turn_eng_off(airplane, cs, rec);
        }
        else
        {
            fprintf(stderr, "The EMERGENCY is not confirmed, the Operation will be aborted\n");
            CMDState emergency = IGNORED;
            rec(airplane, "EMERGENCY", emergency);
            return;
        }
    }

    if(((airplane->flight_controls & GROUND_MASK) != 0) && ((airplane->flight_controls & AIRBORN_MASK) == 0))
    {   
        if((airplane->engine_control & LEFT_ENGINE_MASK) == 0 && (airplane->engine_control & RIGHT_ENGINE_MASK) == 0)
        {
            printf("WARNING: Both Left and Right engines are already OFF\n");
            CMDState state = IGNORED;
            rec(airplane, "ENG-OFF", state);
            return;
        }

        turn_eng_off(airplane, cs, rec);
    }
}

/**
 * @brief Requests Control Tower for landing
 * @param Airplane airplane pointer
 * @return uint8_t
 */
uint8_t request_landing_auth(Airplane *airplane)
{
    if(((airplane->flight_controls & LANDING_GEAR_MASK) != 0) && (airplane->flight_controls & AIRBORN_MASK) != 0)
    {
        printf("Flight %s is authorized for landing\n", airplane->id);
        return 1;
    }
    else
    {
        fprintf(stderr, "ALERT: Execute Landing Configuration DENIED. Airplane Configuration NOT VALID. Contact the Control Tower\n");
        return 0;
    }
}


/**
 * @brief Executes the landing configuration and lands the airplane
 * @param Airplane airplane pointer
 * @param void rec pointer callback
 * @return void
 */
void execute_landing(Airplane *airplane, void(*rec)(Airplane *airplane, char *cmd, CMDState cmd_state))
{
    printf("EXEC: Configure Airplane for landing\n");
    
    if(request_landing_auth(airplane) == 1)
    {
        airplane->flight_controls &= ~SWITCH(LANDING_GEAR_BIT);
        airplane->flight_controls |= LANDING_GEAR_MASK;
        
        if((airplane->flight_controls & LANDING_GEAR_MASK) == 0) 
        {
            fprintf(stderr, "ALERT: Problem retreating LANDING GEAR. Go Around Required");
            exit(1);
        }

        CMDState state = ON;
        rec(airplane, "LAND-GEAR", state);
        
        // Automatically toggle off the AIRBORN state  
        airplane->flight_controls &= ~SWITCH(AIRBORNE_BIT);
        
        if((airplane->flight_controls & AIRBORN_MASK) != 0)
        {
            fprintf(stderr, "ALERT: Wrong Airplane STATE. Expected AIRBORN to be OFF\n");
            exit(1);
        }

        printf("Fligh %s successfully landed\n", airplane->id);
        CMDState landed_state = ON;
        rec(airplane, "LANDED", landed_state);
      
        // Automatically toggle ON GROUND state 
        airplane->flight_controls |= SWITCH(GROUND_BIT);

        if((airplane->flight_controls & GROUND_MASK) == 0)
        {
            fprintf(stderr, "ALERT: Wrong Airplane STATE. Expected GROUND to be ON\n");
            exit(1);
        }

        CMDState ground_state = ON;
        rec(airplane, "IS-GROUD", ground_state);
    }
    else
    {
        fprintf(stderr, "Authorization DENIED, please go around and perform the landing checklist again\n");
        exit(1);
    }
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
    // Possible Emergency 
    toggle_engine_switch_off(plane, LEFT, record_command);
    
    execute_landing(plane, record_command); 
    show_flight_recordings(plane);
    return 0;
}
