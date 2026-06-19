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
#define RIGHT_ENGINE_MASK 0x4 
// Airplane Authorization
#define AUTHORIZATION_MASK 0x8F
#define AUTH_GROUND 0x00
#define AUTH_FUEL 0x02
#define AUTH_L_ENG 0x04
#define AUTH_R_ENG 0x80 

typedef struct
{
    /**
     * bit 0 = onground
     * bit 2 = airborn
     * bit 4 = fuel status
     */
    uint8_t flight_controls;
    /**
     * bit 2 = engine left status
     * bit 4 = engine right status 
     */
    uint8_t engine_control;
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
    uint8_t authorized_to_flight;
    char *id;
} Airplane;

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

    *airplane = plane;    
}

/**
 * @brief Refills the Airplane's tanks
 * @param Airplane airplane pointer
 * @return void
 */
void refill(Airplane *airplane)
{
    if((airplane->flight_controls & SWITCH(FUEl_MASK)) == 0)
    {
        printf("Re-filling Airplane's Tanks\n");
        airplane->flight_controls |= SWITCH(FUEl_MASK);
        airplane->authorized_to_flight |= SWITCH(AUTH_FUEL);
    }
    else
    {
        printf("The Airplane's Tanks are already filled\n");
    }
}

int main(void)
{
    Airplane *plane = NULL;
    init(&plane, "WA-777");
    refill(plane);
    return 0;
}
