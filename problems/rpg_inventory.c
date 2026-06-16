#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define WEAPON 0
#define CONSUMABLE 1
#define CAPACITY 10 
#define BREAK printf("---------------------------------\n");
// Change state Macro 
#define BIT(x) (1 << (x))
// Item State Macros
#define ITEM_RESET_MASK 0x00 
#define ITEM_QUEST_IDX 0
#define ITEM_BROKEN_IDX 1
#define ITEM_EQUIP_IDX 3
#define ITEM_ADDED_IDX 7
#define ITEM_EQUIP_MASK 0x8F
// ItemDetail Masks
#define ITEM_D_MASK_LEFT 0xF0
#define ITEM_D_MASK_RIGHT 0x0F
#define ITEM_D_COMBINE(left, right) ((((uint8_t)(left) << 4) & ITEM_D_MASK_LEFT) | ((uint8_t)(right) & ITEM_D_MASK_RIGHT))
#define ITEM_D_EXTRACT_LEFT(val) (((uint8_t)(val) & ITEM_D_MASK_LEFT) >> 4)
#define ITEM_D_EXTRACT_RIGHT(val) ((uint8_t)(val) & ITEM_D_MASK_RIGHT)

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
    // 4 bits damage & 4 bits durability
    uint8_t wepaon_detail;
    // 4 bits magicka cost - 4 bits health points
    uint8_t consumable_detail;
};

typedef struct Item
{
    char *name;
    // Added, Equipped, Broken, Quest Item.
    // Quest Item = 0 
    // Broken = 2 
    // Equipped = 4 
    // Placed in the Inventory = 8
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
 * @param uint8_t capacity The size of the hash table array.
 * @return uint8_t The calculated index.
 */
uint8_t hash_function(const char *key, uint8_t capacity) {
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
            dur = 8;
            break;
        case BOW:
            pow = 4;
            dur = 7;
            break;
        case DAGGER:
            pow = 2;
            dur = 4;
            break;
        case SWORD:
            pow = 10;
            dur = 15;
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
            health = 8;
            cost = 4;
            break;
        case SNACK:
            health = 4;
            cost = 2;
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

    new_inv->capacity = CAPACITY;
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
    // create two variables to hold the 4 bits damage & 4 bits damage
    uint8_t left_value = stats[0];
    uint8_t right_value = stats[1];

    Item *item = (Item *)malloc(sizeof(Item));
    
    if(item == NULL)
    {
        fprintf(stderr, "Could not allocate space for the new weapon\n");
        exit(1);
    }
    
    item->next = NULL;
    // Before adding a new weapon sets the flag to all 0s
    item->item_state = ITEM_RESET_MASK;
    item->item_detail.wepaon_detail = ITEM_RESET_MASK;
    
    // get the index from the hash_function
    uint8_t idx = hash_function(weapon_name, CAPACITY);    
    
    // copy the weapon_name 
    int weapon_name_size = strlen(weapon_name) + 1;
    item->name = (char *)malloc(sizeof(char) * weapon_name_size);
    
    if(item->name == NULL)
    {
        fprintf(stderr, "Could not allocate space for the weapon's name\n");
        exit(1);
    }
    
    strncpy(item->name, weapon_name, weapon_name_size);
    item->name[weapon_name_size - 1] = '\0';
   
    // Set default Item State (Item picked up and added to inventory)
    item->item_state |= BIT(ITEM_ADDED_IDX);
    // assign the 4 bits damage & 4 bits damage
    item->item_detail.wepaon_detail = ITEM_D_COMBINE(left_value, right_value);

    if(inventory->items[idx] == NULL)
    {
        inventory->items[idx] = item;
    }
    else
    {
        Item *current = inventory->items[idx];
        while(current->next != NULL) current = current->next;
        current->next = item;
    }
    
    inventory->items_stored += 1;
    free(stats);
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
    uint8_t left_value = cons_stats[0];
    uint8_t right_value = cons_stats[1];
    
    Item *item = (Item *)malloc(sizeof(Item));

    if(item == NULL)
    {
        fprintf(stderr, "Could not allocate space for the new consumable\n");
        exit(1);
    }

    item->next = NULL;
    item->item_state = ITEM_RESET_MASK;
    item->item_detail.wepaon_detail = ITEM_RESET_MASK;

    uint8_t idx = hash_function(consumable_name, CAPACITY);

    int consumable_name_size = strlen(consumable_name) + 1;
    item->name = (char *)malloc(sizeof(char) * consumable_name_size);

    if(item->name == NULL)
    {
        fprintf(stderr, "Could not allocate space for the consumable_name's name\n");
        exit(1);
    }

    strncpy(item->name, consumable_name, consumable_name_size);
    item->name[consumable_name_size - 1] = '\0';

    item->item_state |= BIT(ITEM_ADDED_IDX);
    item->item_detail.consumable_detail = ITEM_D_COMBINE(left_value, right_value);

    if(inventory->items[idx] == NULL)
    {
        inventory->items[idx] = item;
    }
    else
    {
        Item *current = inventory->items[idx];
        while(current->next != NULL) current = current->next;
        current->next = item;
    }

    inventory->items_stored += 1;
    free(cons_stats);
}

/**
 * @brief Finds a weapon by name
 * @param Inventory inventory pointer
 * @param char weapon_name pointer
 * @return Item pointer
 */
Item *find_item(Inventory *inventory, char *item_name)
{
    uint8_t idx = hash_function(item_name, CAPACITY);
    
    if(inventory->items[idx] == NULL)
    {
        fprintf(stderr, "Could not find the item '%s' for in your inventory\n", item_name);
        exit(1);
    }
    else
    {
        Item *head = inventory->items[idx];

        if(strcmp(head->name, item_name) == 0) return head;
        
        else
        {
            while(head != NULL)
            {
                if(strcmp(head->name, item_name) == 0) return head;
                head = head->next;
            }
        }
    }

    return NULL;
}

/**
 * @brief Equip a weapon from inventory
 * @param Inventory inventory pointer
 * @param char weapon_name pointer
 * @return void
 */
void equip_weapon(Inventory *inventory, char *weapon_name)
{
    Item *weapon = find_item(inventory, weapon_name);  
    
    if((weapon->item_state & BIT(ITEM_EQUIP_IDX)) != 0)
    {
        printf("'%s' is already equipped\n", weapon_name);
    }
    else
    {
        weapon->item_state = ITEM_EQUIP_MASK;
        weapon->item_state |= BIT(ITEM_EQUIP_IDX);
        
        if((weapon->item_state & BIT(ITEM_EQUIP_IDX)) != 0)
        {
            printf("'%s' equipped successfully\n", weapon_name);
            // extract the stats from the selected weapon
            uint8_t pow = ITEM_D_EXTRACT_LEFT(weapon->item_detail.wepaon_detail);
            uint8_t dur = ITEM_D_EXTRACT_RIGHT(weapon->item_detail.wepaon_detail);
            printf("Power     : %d\n", pow);
            printf("Durability: %d\n", dur);
        }
        else
        {
            printf("There was an issue equipping '%s'\n", weapon_name);
        }
    }

    BREAK;
}

/**
 * @brief Prints the current player's stats 
 * @param uint8_t ph 
 * @param uint8_t pm 
 * @return void
 */
void print_player_stats(uint8_t ph, uint8_t pm)
{
    printf("--- Player's Stats ---\n");
    printf("Health : %d\n", ph);
    printf("Magicka: %d\n", pm);
    BREAK;
}

/**
 * @brief Consumes an item from the inventory
 * @param Inventory inventory pointer
 * @param char consumable_name pointer
 * @param uint8_t ph pointer
 * @param uint8_t pm pointer
 * @return void
 */
void use_consumable(Inventory *inventory, char *consumable_name, uint8_t *ph, uint8_t *pm, void(*on_consume)(uint8_t ph, uint8_t pm))
{
    Item *consumable = find_item(inventory, consumable_name);

    uint8_t health = ITEM_D_EXTRACT_LEFT(consumable->item_detail.consumable_detail);
    uint8_t magicka = ITEM_D_EXTRACT_RIGHT(consumable->item_detail.consumable_detail);
   
    // check if the player has enough magicka to consume the selected item
    if(*pm < magicka)
    {
        fprintf(stderr, "Not enough  magicka to consume the item\n");
        return;
    }

    *ph += health;
    *pm -= magicka;
    inventory->items_stored -= 1; 

    on_consume(*ph, *pm);
}

/**
 * @brief Clens the allocated objects from Memory
 * @param Inventory inventory pointer
 * @return void
 */
void clean_mem(Inventory *inventory)
{
    for(int i = 0; i < CAPACITY; i++)
    {
        if(inventory->items[i] != NULL)
        {
            Item *current = inventory->items[i];
            Item *next_item = NULL;

            while(current != NULL)
            {
                next_item = current->next;
                free(current->name);
                free(current);
                current = next_item;
            }
        }
    }

    free(inventory->items);
    free(inventory);
    inventory = NULL;
}


int main(void)
{
    uint8_t player_health = 20;
    uint8_t player_magicka = 10;
    Inventory *inventory = NULL;
    init_inventory(&inventory);

    enum Weapons sword = SWORD;
    enum Weapons dagger = DAGGER;
    enum Weapons axe = AXE;
    enum Consumables potion = POTION;
    enum Consumables dry_meat = SNACK;
    
    // Add weapons 
    add_weapon(inventory, "Sword", sword);
    add_weapon(inventory, "Dagger", dagger);
    add_weapon(inventory, "Dawnbreaker", axe);
    
    // Equip a Weapon
    equip_weapon(inventory, "Dawnbreaker");
    equip_weapon(inventory, "Sword");
    equip_weapon(inventory, "Sword");
    
    // Add Consumables
    add_consumable(inventory, "Health Potion", potion);
    add_consumable(inventory, "Dry Meat", dry_meat);
   
    // Use Consumable
    use_consumable(inventory, "Health Potion", &player_health, &player_magicka, print_player_stats);
    

    // Clean up 
    clean_mem(inventory);
    return 0;
}
