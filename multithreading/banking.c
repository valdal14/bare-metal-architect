#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define BIT(x) (1 << (x))
#define CUS_INFO_DEF_MASK 0x00
// Customer's Macros 
#define LOCK_BIT 0 
#define BALANCE_BIT 1 
#define PREMIUM_BIT 3
// Bank's Macros 
#define BANK_BRANCH_CAPACITY 10

// TYPES -----------------------------------------------------
typedef enum
{
    CUSTOMER,
    ACCOUNT,
    BRANCH,
    BANK 
} OBJType;

typedef struct Customer
{
    char *fullname;

} Customer; 

typedef struct Account
{

    struct Customer *customer;
    uint32_t id;
    /**
     * bit 0 locked
     * bit 2 is balance red
     * bit 4 is premium 
     */
    uint8_t info; 

} Account;

typedef struct Branch 
{
    char *branch_id;
    struct Account *head;
    struct Account *tail;
} Branch;

typedef struct
{
    struct Branch **branches;
    uint8_t branch_count;
    uint8_t capacity;
} Bank;

// METHODS  --------------------------------------------------

/**
 * @brief Maps the given OBJType and return a readable string 
 * @param OBJType type
 * @return char pointer
 */
char *type_to_str(OBJType type)
{
    switch(type)
    {
        case CUSTOMER: return "Customer";
        case ACCOUNT: return "Account";
        case BRANCH: return "Branch";
        case BANK: return "Bank";
        default:
            fprintf(stderr, "Could not identify given type\n");
            exit(EXIT_FAILURE);
    } 
}

/**
 * @brief Helper used to verify the state of the object
 * allocation. 
 *
 * If the allocation fails the program will be terminated
 *
 * @param void obj pointer
 * @param OBJType type
 * @return void
 */
void obj_alloc(void *obj, OBJType type)
{
    char *obj_name = type_to_str(type);
    bool failed = false;

    switch(type)
    {
        case CUSTOMER:
            if((Customer *)obj == NULL) failed = true;
            break;
        case ACCOUNT:
            if((Account *)obj == NULL) failed = true;
            break;
        case BRANCH:
            if((Branch *)obj == NULL) failed = true;
            break;
        case BANK:
            if((Bank *)obj == NULL) failed = true;
            break;
        default:
            fprintf(stderr, "Unsupported type\n");
            exit(EXIT_FAILURE);
    }

    if(failed)
    {
        fprintf(stderr, "Could not allocate space for %s\n", obj_name);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Inits a new Bank with default values
 * @param Bank bank double pointer
 * @return void
 */
void init(Bank **bank)
{
    Bank *new_bank = (Bank *)calloc(1, sizeof(Bank));
    obj_alloc(new_bank, BANK);

    new_bank->branches = (Branch **)calloc(BANK_BRANCH_CAPACITY, sizeof(Branch *));
    obj_alloc(new_bank->branches, BRANCH);

    new_bank->branch_count = 0;
    new_bank->capacity = BANK_BRANCH_CAPACITY;

    *bank = new_bank;
}

int main(void)
{
    Bank *bank = NULL;
    init(&bank);
    printf("Bank opened at address %p\n", bank);
    return 0;
}
