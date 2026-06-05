#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// 1. Define the Union (The Shape-Shifter)
union PosPayload {
    uint32_t raw_packet; // The single 32-bit integer we send over the wire
    uint8_t bytes[4];    // The exact same memory, viewed as 4 individual bytes
};

// 2. The Packer (Using Left Shifts '<<' and OR '|')
uint32_t pack_transaction(uint32_t store, uint32_t reg, uint32_t refund, uint32_t amt) 
{
    uint32_t packet = 0; // Start with 32 zeros: 00000000000000000000000000000000

    // Step A: Mask the inputs to ensure they physically cannot exceed their bit-limits
    store  = store  & 0xFFF;   // Max 12 bits (4095)
    reg    = reg    & 0xF;     // Max 4 bits  (15)
    refund = refund & 0x1;     // Max 1 bit   (1)
    amt    = amt    & 0x7FFF;  // Max 15 bits (32767)

    // Step B: Shift each value into its designated physical location and merge it
    packet |= store;             // Bits 0-11: No shift needed, starts at right edge
    packet |= (reg << 12);       // Bits 12-15: Push left by 12 spaces
    packet |= (refund << 16);    // Bit 16: Push left by 16 spaces
    packet |= (amt << 17);       // Bits 17-31: Push left by 17 spaces

    return packet;
}

// 3. The Unpacker (Using Right Shifts '>>' and AND '&')
void unpack_transaction(uint32_t packet) 
{
    // Step C: Shift the bits back to the far right, then apply the mask to read them
    uint32_t store  = packet & 0xFFF;               // Already at the right edge
    uint32_t reg    = (packet >> 12) & 0xF;         // Pull right by 12, then read 4 bits
    uint32_t refund = (packet >> 16) & 0x1;         // Pull right by 16, then read 1 bit
    uint32_t amt    = (packet >> 17) & 0x7FFF;      // Pull right by 17, then read 15 bits

    printf("\n--- Receiver Engine (Unpacking) ---\n");
    printf("Store ID : %u\n", store);
    printf("Register : %u\n", reg);
    printf("Is Refund: %s\n", refund ? "True" : "False");
    printf("Amount   : $%u\n", amt);
}

int main() 
{
    // Simulate our parsed JSON payload variables
    uint32_t json_store_id = 4095;
    uint32_t json_register = 15;
    uint32_t json_is_refund = 1;
    uint32_t json_amount = 250;

    printf("--- Original JSON Payload ---\n");
    printf("{\"store_id\": %u, \"register\": %u, \"is_refund\": %u, \"amount\": %u}\n", 
           json_store_id, json_register, json_is_refund, json_amount);

    // Compress the data using our bitwise packer
    union PosPayload tx;
    tx.raw_packet = pack_transaction(json_store_id, json_register, json_is_refund, json_amount);

    // Look at the Union in action!
    printf("\n--- Network Transmission Layer ---\n");
    printf("Compressed 32-bit Integer : %u\n", tx.raw_packet);
    printf("Raw Bytes sent over wire  : [ %02X %02X %02X %02X ]\n", 
           tx.bytes[0], tx.bytes[1], tx.bytes[2], tx.bytes[3]);

    // Unpack the data on the other side
    unpack_transaction(tx.raw_packet);

    return 0;
}
