#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ITEM_NAME_SIZE 16
#define CAPACITY 3
#define SLOT_ITEM_CAPACITY 10
#define TYPE_SNACK 0
#define TYPE_DRINK 1
#define TYPE_DEVICE 2 

union ItemDetail {
    uint16_t volume;   
    uint16_t calories; 
};

typedef struct Item {
    char name[ITEM_NAME_SIZE];          
    uint8_t price;       
    // Bit 0: IS_EXPIRED, Bit 1: HAS_WARRANTY
    uint8_t item_flags; 
    union ItemDetail info;
    struct Item *next;   
} Item;

typedef struct Slot {
    char slot_id[3];
    uint8_t slot_flags;
    Item *head;       
    uint8_t slot_capacity; 
    struct Slot *next_broken;
} Slot;

typedef struct VendingMachine {
    Slot grid[CAPACITY];         
    Slot *maintenance_list;
} VendingMachine;

/**
 * @brief Init the VendingMachine and setup the slots 
 * @param VendingMachine vending_machine double pointer
 * @return void
 */
void setup_vm(VendingMachine **vending_machine)
{
    VendingMachine *vm = (VendingMachine *)calloc(1, sizeof(VendingMachine));
    if(vm == NULL) exit(1);
    Slot s1, s2, s3;
    s1.head = NULL;
    s2.head = NULL;
    s3.head = NULL;
    s1.slot_capacity = 0;
    s2.slot_capacity = 0;
    s3.slot_capacity = 0;
    s1.slot_flags = 0;
    s2.slot_flags = 0;
    s3.slot_flags = 0;
    s1.next_broken = NULL;
    s2.next_broken = NULL;
    s3.next_broken = NULL;
    strncpy(s1.slot_id, "A1\0", 3);
    strncpy(s2.slot_id, "A2\0", 3);
    strncpy(s3.slot_id, "A3\0", 3);
    
    vm->grid[0] = s1;
    vm->grid[1] = s2;
    vm->grid[2] = s3;

    *vending_machine = vm;
}

/**
 * @brief Check if there is space to add an item and verifies the allocation of a new item 
 * @param Item current pointer
 * @param uint8_t current_capacity
 * @param int slot_type
 * @return void
 */
void check_slot_availability(Item *current, uint8_t current_capacity, int slot_type)
{
    char str[7];

    switch(slot_type)
    {
        case TYPE_SNACK:
            strncpy(str, "Snack\0", 6);
            break;
        case TYPE_DRINK:
            strncpy(str, "Drink\0", 6);
            break;
        case TYPE_DEVICE:
            strncpy(str, "Device\0", 7);
            break;
        default:
            fprintf(stderr, "Unsupported type\n");
            exit(1);
    }

    if(current_capacity + 1 > SLOT_ITEM_CAPACITY)
    {
        fprintf(stderr, "The %s slot is full\n", str);
        exit(1);
    }

    if(current == NULL)
    {
        fprintf(stderr, "Could not allocate space for a new %s\n", str);
        exit(1);
    }
}

/**
 * @brief Add new Snack to the VendingMachine's slot
 * @param VendingMachine vending_machine pointer
 * @param char name pointer
 * @param uint8_t price
 * @param uint16_t calories
 * @return void
 */
void add_snack(VendingMachine *vending_machine, char *name, uint8_t price, uint16_t calories)
{
    Item *new_snack = (Item *)calloc(1, sizeof(Item));
    check_slot_availability(new_snack, vending_machine->grid[TYPE_SNACK].slot_capacity, TYPE_SNACK);

    strncpy(new_snack->name, name, ITEM_NAME_SIZE);
    new_snack->name[ITEM_NAME_SIZE - 1] = '\0';
    new_snack->price = price;
    // we do not add expired items when we load the machine...
    new_snack->item_flags |= (0 << 0);
    new_snack->info.calories = calories;
    new_snack->next = NULL;

    if(vending_machine->grid[TYPE_SNACK].head == NULL)
    {
        vending_machine->grid[TYPE_SNACK].head = new_snack;    
    }
    else
    {
        Item *current = vending_machine->grid[TYPE_SNACK].head;
        while(current->next != NULL) current = current->next;
        current->next = new_snack;
    }

    vending_machine->grid[TYPE_SNACK].slot_capacity += 1;
}

/**
 * @brief Add new Drink to the VendingMachine's slot
 * @param VendingMachine vending_machine pointer
 * @param char name pointer
 * @param uint8_t price
 * @param uint16_t volume
 */
void add_drink(VendingMachine *vending_machine, char *name, uint8_t price, uint16_t volume)
{
    Item *new_drink = (Item *)calloc(1, sizeof(Item));
    check_slot_availability(new_drink, vending_machine->grid[TYPE_DRINK].slot_capacity, TYPE_DRINK);

    strncpy(new_drink->name, name, ITEM_NAME_SIZE);
    new_drink->name[ITEM_NAME_SIZE - 1] = '\0';
    new_drink->price = price;
    // we do not add expired items when we load the machine...
    new_drink->item_flags |= (0 << 0);
    new_drink->info.volume = volume;
    new_drink->next = NULL;

    if(vending_machine->grid[TYPE_DRINK].head == NULL)
    {
        vending_machine->grid[TYPE_DRINK].head = new_drink;
    }
    else
    {
        Item *current = vending_machine->grid[TYPE_DRINK].head;
        while(current->next != NULL) current = current->next;
        current->next = new_drink;
    }

    vending_machine->grid[TYPE_DRINK].slot_capacity += 1;
}

/**
 * @brief Add new Device to the VendingMachine's slot
 * @param VendingMachine vending_machine pointer
 * @param char name pointer
 * @param uint8_t price
 * @param uint8_t has_warranty
 * @return void
 */
void add_device(VendingMachine *vending_machine, char *name, uint8_t price, uint8_t has_warranty)
{
    if(has_warranty > 1 || has_warranty < 0)
    {
        fprintf(stderr, "Invalid warranty value provided\n");
        exit(1);
    }

    Item *new_device = (Item *)calloc(1, sizeof(Item));
    check_slot_availability(new_device, vending_machine->grid[TYPE_DEVICE].slot_capacity, TYPE_DEVICE);

    strncpy(new_device->name, name, ITEM_NAME_SIZE);
    new_device->name[ITEM_NAME_SIZE - 1] = '\0';
    new_device->price = price;
    if(has_warranty == 0) new_device->item_flags |= (0 << 1);
    if(has_warranty == 1) new_device->item_flags |= (1 << 1);
    new_device->next = NULL;

    if(vending_machine->grid[TYPE_DEVICE].head == NULL)
    {
        vending_machine->grid[TYPE_DEVICE].head = new_device;
    }
    else
    {
        Item *current = vending_machine->grid[TYPE_DEVICE].head;
        while(current->next != NULL) current = current->next;
        current->next = new_device;
    }
    
    vending_machine->grid[TYPE_DEVICE].slot_capacity += 1;
}


/**
 * @brief Prints out a given VendingMachine's slot
 * @param VendingMachine vending_machine pointer
 * @param int slot_type
 * @return void
 */
void print_slot(VendingMachine *vending_machine, int slot_type)
{
    Item *current = NULL;

    switch(slot_type)
    {
        case TYPE_SNACK:
            printf("SLOT A1 - SNACKS\n");
            printf("%d SNACKS Stored\n", vending_machine->grid[TYPE_SNACK].slot_capacity);
            current = vending_machine->grid[TYPE_SNACK].head;
            break;
        case TYPE_DRINK:
            printf("SLOT A2 - DRINKS\n");
            printf("%d DRINK Stored\n", vending_machine->grid[TYPE_DRINK].slot_capacity);
            current = vending_machine->grid[TYPE_DRINK].head;
            break;
        case TYPE_DEVICE:
            printf("SLOT A3 - DEVICES\n");
            printf("%d DEVICES Stored\n", vending_machine->grid[TYPE_DEVICE].slot_capacity);
            current = vending_machine->grid[TYPE_DEVICE].head;
            break;
        default:
            fprintf(stderr, "Invalid slot type\n");
            exit(1);
    }

    while(current != NULL)
    {
        printf("-----------------------\n");
        printf("Item: %s\n", current->name);
        printf("Price: %d$\n", current->price);
        if(slot_type == TYPE_SNACK) printf("Calories: %d\n", current->info.calories);
        if(slot_type == TYPE_DRINK) printf("Volume: %d ml\n", current->info.volume);
        if(slot_type == TYPE_SNACK && (current->item_flags & (1 << 0)))
            printf("Expired: True\n");
        if(slot_type == TYPE_DEVICE && (current->item_flags & (1 << 1)))
            printf("Warranty: True\n");
        printf("-----------------------\n");
        current = current->next;
    }
}

/**
 * @brief Remove the allocated objects
 * @param VendingMachine vending_machine pointer
 * @return void
 */
void clean_vm(VendingMachine *vending_machine)
{
    for(int i = 0; i < CAPACITY; i++)
    {
        Item *current = vending_machine->grid[i].head;
        Item *next_node = NULL;

        while(current != NULL)
        {
            next_node = current->next;
            free(current);
            current = next_node;
        }
    }
    
    free(vending_machine);
    vending_machine = NULL;
}

int main(void)
{
    VendingMachine *vm = NULL;
    setup_vm(&vm);
    add_snack(vm, "Choco Bar", 2, 246);
    add_snack(vm, "Cookies", 7, 268);
    add_snack(vm, "Fried Chips", 17, 453);
    print_slot(vm, TYPE_SNACK);
    
    add_drink(vm, "Coca Cola", 2, 250); 
    add_drink(vm, "Soda", 1, 150);
    print_slot(vm, TYPE_DRINK);

    add_device(vm, "Pixel 4", 199, 1);
    add_device(vm, "Used iPhone 11", 250, 0);
    add_device(vm, "Galaxy Buds Pro", 245, 1);
    print_slot(vm, TYPE_DEVICE);

    clean_vm(vm);
    return 0;
}
