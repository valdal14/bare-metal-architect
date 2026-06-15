#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define WEAPON 0
#define CONSUMABLE 1

union ItemDetail
{
    // 4 bits durability - 4 bits damage
    uint8_t wepaon_detail;
    // 4 bits magicka cost - 4 bits health points
    uint8_t consumable_detail;
};

typedef struct Item
{
    uint8_t item_id;
    uint8_t item_state;
    union ItemDetail item_detail; 
    struct Item *next;
} Item;

typedef struct Inventory
{
    uint8_t capacity;
    uint8_t items_stored;
    Item **items;
} Inventory;

/**
 * @brief Instanciate a new player's Inventory
 * @param Inventory inventory double pointer
 * @return void
 */
void init_inventory(Inventory **inventory)
{
    Inventory *new_inv = (Inventory *)calloc(1, sizeof(Inventory));
    
    if(new_inv == NULL)
    {
        fprintf(stderr, "Could not instanciate the inventory\n");
        exit(1);
    }

    new_inv->capacity = 10;
    new_inv->items_stored = 0;
    new_inv->items = (Item **)calloc(1, sizeof(Item) * new_inv->capacity);

    if(new_inv->items == NULL)
    {
        fprintf(stderr, "Could not instanciate the space for the Inventory's items\n");
        exit(1);
    }
    
    *inventory = new_inv;
}

int main(void)
{
    Inventory *inventory = NULL;
    init_inventory(&inventory);
    printf("Allocated at %p\n", inventory);
    return 0;
}
