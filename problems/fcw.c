#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

#define PLANE_DEFAULT_MASK 0x00
#define SWITCH(x) (1 << (x))
// Airplane Controls Masks 
#define GROUND_MASK 0x00 
#define AIRBORN_MASK 0x02
#define FUEl_MASK 0x04 
// Engine Controls Masks
#define LEFT_ENGINE_MASK 0x02 
#define RIGHT_ENGINE_MASK 0x04 
// Airplane Authorization
#define AUTHORIZATION_MASK 0x8F
#define AUTH_GROUND 0x00
#define AUTH_FUEL 0x02
#define AUTH_L_ENG 0x04
#define AUTH_R_ENG 0x80 

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
 * @brief Prepare the Airplane
 * @param Airplane double pointer
 * @return void
 */
void init(Airplane **airplane, char *id)
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
    
    *airplane = plane;    
}

/**
 * @brief Refills the Airplane's tanks
 * @param Airplane airplane pointer
 * @return void
 */
void refill(Airplane *airplane)
{
    if((airplane->flight_controls & FUEl_MASK) == 0)
    {
        printf("EXEC: Re-filling Airplane's Tanks\n");
        airplane->flight_controls |= FUEl_MASK;
        airplane->authorized_to_flight |= AUTH_FUEL;
    }
    else
    {
        printf("The Airplane's Tanks are already filled\n");
    }
}

/**
 * @brief Toggles the Engine Control Switch 
 * @param Airplane airplane pointer
 * @param ControlSwitch cs 
 * @return void
 */
void toggle_engine_switch_on(Airplane *airplane, ControlSwitch cs)
{
    if((airplane->engine_control & LEFT_ENGINE_MASK) != 0 && (airplane->engine_control & RIGHT_ENGINE_MASK) != 0)
    {
        printf("WARNING: Both Left and Right engines are already ON\n");
        return;
    }

    switch(cs)
    {
        case LEFT:
        {
            if((airplane->engine_control & LEFT_ENGINE_MASK) == 0 && (airplane->engine_control & GROUND_MASK) == 0)
            {
                printf("EXEC: Toggle ON Left Control Engine Switch\n");
                airplane->engine_control |= LEFT_ENGINE_MASK;
                airplane->authorized_to_flight |= AUTH_L_ENG;
            }
            else
            {
                fprintf(stderr, "ALERT: An attempt to turn the LEFT engine ON failed. Departure aborted\n");
                exit(1);
            }
            break;
        }
        case RIGHT:
        {
            if((airplane->engine_control & RIGHT_ENGINE_MASK) == 0 && (airplane->engine_control & GROUND_MASK) == 0)
            {
                printf("EXEC: Toggle ON Right Control Engine Switch\n");
                airplane->engine_control |= RIGHT_ENGINE_MASK;
                airplane->authorized_to_flight |= AUTH_R_ENG;
            }
            else 
            {
                fprintf(stderr, "ALERT: An attempt to turn the RIGHT engine ON failed. Departure aborted\n");
                exit(1);
            }
            break;
        }
        default:
            fprintf(stderr, "CODE: Invalid ControlSwitch, the Operation will be aborted\n");
            exit(1);
    }
}

int main(void)
{
    Airplane *plane = NULL;
    init(&plane, "WA-777");
    refill(plane);
    toggle_engine_switch_on(plane, RIGHT);
    toggle_engine_switch_on(plane, LEFT);
    return 0;
}
