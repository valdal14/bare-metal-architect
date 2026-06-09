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

int hash_function(const char *key, int capacity) {
    int hash = 0;
    while (*key != '\0') {
        hash = (hash + *key) % capacity;
        key++;
    }
    return hash;
}

void add_snack(VendingMachine *vending_machine, char *name, uint8_t price, uint16_t calories)
{
    if(vending_machine->grid[TYPE_SNACK].slot_capacity + 1 > SLOT_ITEM_CAPACITY)
    {
        fprintf(stderr, "The Snack slot is full\n");
        return;
    }

    Item *new_snack = (Item *)malloc(sizeof(Item));
    if(new_snack == NULL) 
    {
        fprintf(stderr, "Could not allocate space for a new snack\n");
        exit(1);
    }

    strncpy(new_snack->name, name, ITEM_NAME_SIZE);
    new_snack->name[ITEM_NAME_SIZE - 1] = '\0';
    new_snack->price = price;
    // we do not add expired items when we load the machine...
    new_snack->item_flags = 0;
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

/*
void add_drink(VendingMachine *vending_machine, char *name, uint8_t price, uint16_t calories, uint16_t volume)
{

}

void add_device(VendingMachine *vending_machine, char *name, uint8_t price, uint8_t has_warranty)
{
    
}
*/

void print_slot(VendingMachine *vending_machine, int slot_type)
{
    Item *current = NULL;

    switch(slot_type)
    {
        case TYPE_SNACK:
            printf("SLOT A1 - SNACKS\n");
            current = vending_machine->grid[TYPE_SNACK].head;
            break;
        case TYPE_DRINK:
            printf("SLOT A2 - DRINKS\n");
            current = vending_machine->grid[TYPE_DRINK].head;
            break;
        case TYPE_DEVICE:
            printf("SLOT A3 - DEVICES\n");
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
        printf("Price: %d\n", current->price);
        printf("-----------------------\n");
        current = current->next;
    }
}

int main(void)
{
    VendingMachine *vm = NULL;
    setup_vm(&vm);
    add_snack(vm, "Choco Bar", 12, 246);
    add_snack(vm, "Cookies", 14, 268);
    add_snack(vm, "Fried Chips", 17, 453);
    print_slot(vm, TYPE_SNACK);
    return 0;
}
