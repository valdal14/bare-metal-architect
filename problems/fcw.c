#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

#define PLANE_DEFAULT_MASK 0x00
#define SWITCH(x) (1 << (x))
// Airplane Controls Masks 
#define GROUND 0x00 
#define AIRBORN 0x01
#define AUTHORIZATION_MASK 0x80

typedef struct
{
    /**
     * bit 0 = onground
     * bit 2 = aitborn
     * bit 4 = fuel status
     * bit 8 = authorized
     */
    uint8_t flight_controls;
    /**
     * bit 2 = engine left status
     * bit 4 = engine right status 
     */
    uint8_t engine_control;
    char *id;
} Airplane;

/**
 * @brief Toggle the Airplane's Switch 
 * @param Airplane airplane pointer
 * @param uint8_t control_mask
 * @param uint8_t switch_id
 * @return void
 */
void toggle_switch(Airplane *airplane, uint8_t control_mask,  uint8_t switch_id)
{
    
}

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

int main(void)
{
    Airplane *plane = NULL;
    init(&plane, "WA-777");
    printf("Allocated at %p\n", plane);
    printf("Identifier = %s\n", plane->id);
    return 0;
}
