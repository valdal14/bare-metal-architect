#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

float limit(uint32_t r)
{
    return 98.0f / (1.0f + (120.0f / (float)r));
}

int main(void)
{
    uint32_t counter = 1000000;
    
    for(uint32_t i = 1; i < counter; i++)
    {
        float res = limit(i);
        printf("value = %d | limit = %.2f\n", i, res);
        usleep(100000);
    }

}
