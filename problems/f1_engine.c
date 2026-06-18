#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct
{
    // 0 - 7
    double timestamp; 
    // 8 - 11
    uint32_t raw_payload; 
    // 9
    uint8_t checksum; 
    // 10 - 11 
    uint16_t sensor_id; 
    // 12 
    uint8_t status_flag;
    // Total Bytes: 16 (double 8 x 2 = 16 no Tail padding)

} TelemetryPacket;

int main(void)
{
    return 0;
}
