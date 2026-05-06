#include <stdio.h>
#include <stdint.h>

// ==========================================
// 1. THE EVENT HANDLERS
// ==========================================
void handle_syn(void) {
    printf("[SYSTEM] SYN received: Opening new connection...\n");
}

void handle_ack(void) {
    printf("[SYSTEM] ACK received: Packet acknowledged by client.\n");
}

void handle_fin(void) {
    printf("[SYSTEM] FIN received: Tearing down connection...\n");
}

// ==========================================
// 2. THE ROUTER (The Bitwise & Pointer Logic)
// ==========================================
void dispatch_packet(uint8_t packet_flag)
{
    // Step 1: Declare a function pointer called 'event_handler' 
    // It must return void, and take void (no parameters).
    // Initialize it to NULL so it doesn't point to garbage!
    void (*event_handler)(void) = NULL;
    // Step 2: Use Bitwise AND to check the packet_flag.
    // If Bit 0 is 1, point event_handler to handle_syn and execute it.
    // If Bit 1 is 1, point event_handler to handle_ack and execute it.
    // If Bit 2 is 1, point event_handler to handle_fin and execute it.
    if(packet_flag & (1 << 0)) 
    {
        event_handler = &handle_syn;
        event_handler();
    }

    if(packet_flag & (1 << 1))
    {
        event_handler = &handle_ack;
        event_handler();
    }

    if(packet_flag & (1 << 2))
    {
        event_handler = &handle_fin;
        event_handler();
    }
}

// ==========================================
// 3. THE SIMULATION 
// ==========================================
int main(void)
{
    // Scenario 1: A packet arrives with only Bit 0 turned on (0000 0001)
    // Decimal value: 1
    printf("--- Receiving Packet 1 ---\n");
    dispatch_packet(1); 

    // Scenario 2: A packet arrives with only Bit 2 turned on (0000 0100)
    // Decimal value: 4
    printf("\n--- Receiving Packet 2 ---\n");
    dispatch_packet(4);

    // Scenario 3: A packet arrives with BOTH Bit 0 and Bit 1 turned on (0000 0011)
    // Decimal value: 3. (This should trigger BOTH functions sequentially!)
    printf("\n--- Receiving Packet 3 ---\n");
    dispatch_packet(3);

    return 0;
}
