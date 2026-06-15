#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define WEAPON 0
#define CONSUMABLE 1

enum Weapons
{
    AXE,
    BOW,
    DAGGER,
    SWORD
};

enum Consumables
{
    POTION,
    SNACK
};

union ItemDetail
{
    // 4 bits durability - 4 bits damage
    uint8_t wepaon_detail;
    // 4 bits magicka cost - 4 bits health points
    uint8_t consumable_detail;
};

typedef struct Item
{
    char *name;
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
 * @brief Hashes a string key into a valid array index.
 * @param key The string to hash.
 * @param capacity The size of the hash table array.
 * @return int The calculated index.
 */
int hash_function(const char *key, int capacity) {
    int hash = 0;
    while (*key != '\0') {
        hash = (hash + *key) % capacity;
        key++;
    }
    return hash;
}

/**
 * @brief Maps the weapon to its stats 
 * @param enum Weapon 
 * @return uint8_t pointer
 */
uint8_t *map_weapon_stats(enum Weapons weapon)
{
    uint8_t pow = 0;
    uint8_t dur = 0;
    uint8_t *weapon_stats = (uint8_t *)calloc(1, sizeof(uint8_t) * 2);
    
    if(weapon_stats == NULL)
    {
        fprintf(stderr, "Could not allocate space for the Weapon's stats\n");
        exit(1);
    }

    switch(weapon)
    {
        case AXE:
            pow = 15;
            dur = 112;
            break;
        case BOW:
            pow = 8;
            dur = 112;
            break;
        case DAGGER:
            pow = 4;
            dur = 128;
            break;
        case SWORD:
            pow = 10;
            dur = 240;
            break;
        default:
            fprintf(stderr, "Unsupported WEAPON\n");
            exit(1);
    }

    weapon_stats[0] = pow;
    weapon_stats[1] = dur;
    return weapon_stats;
}

/**
 * @brief Maps the consumable to its stats
 * @param enum Consumables consumable
 * @return uint8_t pointer
 */
uint8_t *map_consumable(enum Consumables consumable)
{
    uint8_t health = 0;
    uint8_t cost = 0;

    uint8_t *consumable_stats = (uint8_t *)calloc(1, sizeof(uint8_t) * 2);

    if(consumable_stats == NULL)
    {
        fprintf(stderr, "Could not allocate space for the Consumables's stats\n");
        exit(1);
    }

    switch(consumable)
    {
        case POTION:
            health = 128;
            cost = 8;
            break;
        case SNACK:
            health = 16;
            cost = 112;
            break;
        default:
            fprintf(stderr, "Unsupported CONSUMABLE\n");
            exit(1);
    }

    consumable_stats[0] = health;
    consumable_stats[1] = cost;

    return consumable_stats;
}

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

/**
 * @brief Adds a new item to the Inventory
 * @param Inventory inventory pointer
 * @param char weapon_name pointer
 * @param enum Weapons weapon
 * @return void
 */
void add_weapon(Inventory *inventory, char *weapon_name, enum Weapons weapon)
{
    uint8_t *stats = map_weapon_stats(weapon);

    for(int i = 0; i < 2; i++)
    {
        printf("%d\n", stats[i]);
    }
}

/**
 * @brief Adds a new weapon to the inventory
 * @param Inventory inventory pointer
 * @param char consumable_name pointer
 * @param enum Consumables consumable
 * @return void 
 */
void add_consumable(Inventory *inventory, char *consumable_name, enum Consumables consumable)
{
    uint8_t *cons_stats = map_consumable(consumable);
    
    for(int i = 0; i < 2; i++)
    {
        printf("%d\n", cons_stats[i]);
    }
}

int main(void)
{
    Inventory *inventory = NULL;
    init_inventory(&inventory);
    enum Weapons weapon = SWORD;
    enum Consumables consumable = POTION;
    add_weapon(inventory, "Sword", weapon);
    add_consumable(inventory, "Health Potion", consumable);
    return 0;
}
