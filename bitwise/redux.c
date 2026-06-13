#include <stdio.h>
#include <stdint.h>

#define ROUTER_PORTS 8
#define BIT(x) (1 << (x))

void lockdown_ports(uint8_t *firewall, void(*on_completion)(uint8_t *firewall))
{
    printf("Locking ports down\n");
    *firewall &= ~(BIT(2) | BIT(7));
    on_completion(firewall);
}

void check_port_status(uint8_t *firewall)
{
    for(int i = 0; i < ROUTER_PORTS; i++)
    {
        if((*firewall & BIT(i)) != 0) 
            printf("Port [%d] is turned ON\n", i);
        else
            printf("Port [%d] is turned OFF\n", i);
    }
}

int main(void)
{
    uint8_t firewall = 0xFF;
    lockdown_ports(&firewall, check_port_status);

    return 0;
}
