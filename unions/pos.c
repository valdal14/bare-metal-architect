#include <stdio.h>
#include <stdint.h>

typedef struct
{
    uint32_t store_id;
    uint32_t register_id;
    uint32_t is_refund;
    uint32_t amount;

} Transaction;

typedef union
{
    uint32_t transaction;
    uint8_t bytes[4];
} POS;

void perform_transaction(Transaction *transaction, POS *pos)
{
    POS new;
    // set all bits at 0
    new.transaction = 0;
    new.transaction = (transaction->store_id << 0);
    new.transaction |= (transaction->register_id << 12);
    new.transaction |= (transaction->is_refund << 16);
    new.transaction |= (transaction->amount << 17);
    
    *pos = new;
}

void request_refund(POS *pos)
{
    uint32_t payload = pos->transaction;
    uint32_t is_refunded = (payload >> 16) & 0x1;

    if(is_refunded == 0)
    {
        POS new;
        new.transaction |= (1 << 16);
        *pos = new;
    }
    else
    {
        printf("This transaction cannot be refunded\n");
    }
}

void prinf_invoice(POS *pos)
{
    // Grab the compressed 32-bit payload from the union
    uint32_t payload = pos->transaction;

    // Shift Right (>>) and Mask (&) to extract the variables
    uint32_t recovered_store    = (payload >> 0)  & 0xFFF;
    uint32_t recovered_register = (payload >> 12) & 0xF;
    uint32_t recovered_refund   = (payload >> 16) & 0x1;
    uint32_t recovered_amount   = (payload >> 17) & 0x7FFF;

    printf("\n--- Receiver Engine (Unpacking) ---\n");
    printf("Store ID      : %u\n", recovered_store);
    printf("Register      : %u\n", recovered_register);
    printf("Is Refund     : %s\n", recovered_refund ? "True" : "False");

    printf("Amount        : $%.2f\n", recovered_amount / 100.0);
}

int main(void)
{
    Transaction incoming_transaction = {1050, 8, 0, 1499};
    
    POS pos;
    perform_transaction(&incoming_transaction, &pos); 
    prinf_invoice(&pos);

    request_refund(&pos);
    prinf_invoice(&pos);

    return 0;
}
