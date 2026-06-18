#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define ENGINE_STR_SIZE 6
#define DB_CAPACITY 8
#define BIT(x) (1 << (x))
#define DEFAULT_FLAG_MASK 0x0F

typedef struct
{
    // 0 - 7 = (8 bytes)
    double timestamp; 
    // 8 - 11 = (4 bytes)
    uint32_t raw_payload; 
    // 12 = (1 byte)
    uint8_t checksum; 
    // 13 = (1 bytes)
    uint8_t status_flag;
    // 14 - 15 = (2 byte)
    uint16_t sensor_id;  
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
    uint8_t size;
} Radio;

typedef struct
{
    TelemetryPacket *dest;
    uint8_t size;
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
    float result;
    memcpy(&result, &raw_payload, sizeof(float));
    return result;
}

/**
 * @brief Reads packets from the source and copies 
 * the packets into the analytics.
 * @param Radio radio pointer
 * @param const Analytics analytics pointer
 * @return void
 */
void batch_copy(Radio *radio, Analytics *analytics)
{
    for (size_t i = 0; i < radio->size; i++) 
        memmove((TelemetryPacket *)&analytics->dest[i], (TelemetryPacket *)&radio->src[i], sizeof(TelemetryPacket)); 
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
            {
                Radio *radio = (Radio *)calloc(1, sizeof(Radio));
                if(radio == NULL)
                {
                    fprintf(stderr, "Could not allocate space for the new RADIO\n");
                    exit(1);
                }
                radio->size = 0;
                radio->src = db;
                *engine = radio;
                break;
            }
            case ANALY:
            {
                Analytics *analytics = (Analytics *)calloc(1, sizeof(Analytics));
                if(analytics == NULL)
                {
                    fprintf(stderr, "Could not allocate space for the new ANALYTIC\n");
                    exit(1);
                }
                analytics->size = 0;
                analytics->dest = db;
                *engine = analytics;
                break;
            }
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

/**
 * @brief Recursively add N packets to the destination TelemetryPacket array by incrementing 
 * the value of each variable for each iteration. 
 * @param Radio radio pointer
 * @param uint16_t id: sensor_id: Must start at 0  
 * @param double ts: timestamp
 * @param uint32_t raw_data: raw_payload
 * @param uint8_t cs: checksum
 * @param uint8_t n: number of iteration
 * @return uint8_t: 0 if failed 1 if success
 */ 
uint8_t add_radio_packet(Radio *radio, uint16_t id, double ts, float raw_data, uint8_t cs, uint8_t n) 
{
    if(n >= DB_CAPACITY)
    {
        fprintf(stderr, "The radio signal batch process size is too big\n");
        exit(1);
    }

    if(n == 0)
    {
        if(radio->size == id)
            return 1;
        else
            return 0;
    }
    
    radio->src[id].sensor_id = id;
    radio->src[id].timestamp = ts + id; 
    radio->src[id].raw_payload = get_tire_temp(raw_data + id); 
    radio->src[id].checksum = cs + id;
    radio->src[id].status_flag = DEFAULT_FLAG_MASK;
    radio->size += 1;
    
    if(n % 2 == 0)
        radio->src[id].status_flag |= BIT(1);
    else
        radio->src[id].status_flag ^= BIT(1);
   
    // Recursively add new packets 
    return add_radio_packet(radio, 
            (id + 1), 
            radio->src[id].timestamp, 
            radio->src[id].raw_payload, 
            radio->src[id].checksum, 
            (n - 1)
    );
}

/**
 * @brief Verifies the result of the Radio batch process
 * @param void pointer
 * @param uint8_t result
 * @param F1Type type 
 * @param uint8_t size 
 * @return void
 */
void verify_batch(void *engine, uint8_t result, F1Type type, uint8_t size)
{
    if(result == 0) 
    {
        fprintf(stderr, "Batch Processed Failed\n");
        exit(1);
    }

    switch(type)
    {
        case RADIO:
            if((Radio *)engine != NULL) 
            {
                printf("RADIO batch process completed with success state\n");
            }
            else
            {
                fprintf(stderr, "RADIO batch process could not be recorded\n");
                exit(1);
            }
            break;
        case ANALY:
            if((Analytics *)engine != NULL)
            {
                printf("RADIO to ANALYTIC batch process copy completed with success state\n");
                printf("\n----------------- ANALYTIC REPORT -----------------\n");
                for(int i = 0; i < size; i++)
                {
                    Analytics *eng = (Analytics *)engine;
                    printf("PACKID[%d] = %d -> ", i, eng->dest[i].sensor_id);
                    printf("STATUS[%d] = %d -> ", i, (eng->dest[i].status_flag & BIT(1)));
                    printf("RAW_PY[%d] = %d\n", i, eng->dest[i].raw_payload);
                }
            }
            else
            {
                fprintf(stderr, "RADIO batch process could not be recorded\n");
                exit(1);
            }
            break;
        default:
            fprintf(stderr, "Unsupported Engine Type Provided\n");
            exit(1);
    }
}

int main(void)
{
    Radio *radio = NULL;
    F1Type t1 = RADIO;
    Analytics *analytics = NULL;
    F1Type t2 = ANALY;
    // Init RADIO
    init((void **)&radio, t1);
    // Init ANALYTIC
    init((void **)&analytics, t2);

    // Batch Radio Process
    uint8_t batch_res = add_radio_packet(radio, 0, 10, 2, 1, 7);
    verify_batch(radio, batch_res, RADIO, radio->size);
    
    // Copy/Move the Radio waves packets to the Analytics Engine
    batch_copy(radio, analytics);
    verify_batch(analytics, batch_res, ANALY, radio->size);

    // cleanup 
    free(radio->src);
    free(radio);
    free(analytics->dest);
    free(analytics);
    return 0;
}
