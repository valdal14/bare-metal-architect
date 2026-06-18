#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define ENGINE_STR_SIZE 6
#define DB_CAPACITY 10 

typedef struct
{
    // 0 - 7 = (8 bytes)
    double timestamp; 
    // 8 - 11 = (4 bytes)
    uint32_t raw_payload; 
    // 9 = (1 byte)
    uint8_t checksum; 
    // 10 - 11 = (2 bytes)
    uint16_t sensor_id; 
    // 12 = (1 byte) 
    uint8_t status_flag;
    // Total Bytes: 16 (double 8 x 2 = 16 no Tail padding)

} TelemetryPacket;

typedef enum 
{
    RADIO = 1,
    ANALY = 2
} F1Type;

typedef struct
{
    TelemetryPacket *src; 
} Radio;

typedef struct
{
    TelemetryPacket *dest;
} Analytics;

/**
 * @brief Safely extract the float temp value
 * without triggering the strict aliasing rule
 * optmiser.
 * @param uint32_t raw_payload
 * @return float
 */
float get_tire_temp(uint32_t raw_payload)
{
   return 0.0;
}

/**
 * @brief Reads packets from the source and copies 
 * the packets into the analytics.
 * @param TelemetryPacket dest pointer
 * @param const TelemetryPacket src pointer
 * @param size_t count 
 * @return void
 */
void batch_copy(TelemetryPacket *dest, const TelemetryPacket *src, size_t count)
{

}

/**
 * @brief Maps the F1Type type value to 
 * a readable output.
 * @param F1Type type
 * @return char pointer
 */
char *map_engine(F1Type type)
{
    char *engine_str = (char *)malloc(sizeof(char) * ENGINE_STR_SIZE);
    
    if(engine_str == NULL)
    {
        fprintf(stderr, "Could not allocate space for the string\n");
        exit(1);
    }

    switch(type)
    {
        case RADIO:
            strncpy(engine_str, "RADIO", ENGINE_STR_SIZE);
            break;
        case ANALY: 
            strncpy(engine_str, "ANALY", ENGINE_STR_SIZE);
            break;
        default:
            fprintf(stderr, "TYPE-ERROR: Unsupported type\n");
            exit(1);
    }

    engine_str[ENGINE_STR_SIZE - 1] = '\0';
    return engine_str;
}

/**
 * @brief Inits either a RADIO or ANALYTIC engine
 * @param void engine double pointer
 * @param F1Type type
 * @return void
 */
void init(void **engine, F1Type type)
{
    if((TelemetryPacket *)engine != NULL)
    {
        TelemetryPacket *db = (TelemetryPacket *)calloc(DB_CAPACITY, sizeof(TelemetryPacket));
        
        if(db == NULL)
        {
            fprintf(stderr, "Could not allocate space for the new engine DB\n");
            exit(1);
        }
        
        switch(type)
        {
            case RADIO:
                printf("RADIO\n");
                Radio *radio = (Radio *)calloc(1, sizeof(Radio));
                if(radio == NULL)
                {
                    fprintf(stderr, "Could not allocate space for the new RADIO\n");
                    exit(1);
                }
                radio->src = db;
                *engine = radio;
                break;
            case ANALY:
                printf("ANALY\n");
                Analytics *analytics = (Analytics *)calloc(1, sizeof(Analytics));
                if(analytics == NULL)
                {
                    fprintf(stderr, "Could not allocate space for the new ANALYTIC\n");
                    exit(1);
                }
                analytics->dest = db;
                *engine = analytics;
                break;
            default:
                fprintf(stderr, "TYPE-ERROR: Unsupported type\n");
                exit(1);
        }
    }
    else
    {
        char *engine_desc = map_engine(type);
        fprintf(stderr, "Could not allocated space for the %s engine\n", engine_desc);
        free(engine);
        exit(1);
    }
}

int main(void)
{
    Radio *radio = NULL;
    F1Type t1 = RADIO;
    Analytics *analytics = NULL;
    F1Type t2 = ANALY;
    init((void **)&radio, t1);
    init((void **)&analytics, t2);
    return 0;
}
