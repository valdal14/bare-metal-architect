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
#define BANK_BRANCH_CAPACITY 5

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
    struct Account *next;
    int balance;
    uint32_t id;
    /**
     * bit 0 locked
     * bit 2 is balance red
     * bit 4 is premium 
     */
    uint8_t info; 

} Account; // 25 + 1 padding bytes 

typedef struct Branch 
{
    char *branch_id;
    struct Branch *next;
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
void init_bank(Bank **bank)
{
    Bank *new_bank = (Bank *)calloc(1, sizeof(Bank));
    obj_alloc(new_bank, BANK);

    new_bank->branches = (Branch **)calloc(BANK_BRANCH_CAPACITY, sizeof(Branch *));
    obj_alloc(new_bank->branches, BRANCH);

    new_bank->branch_count = 0;
    new_bank->capacity = BANK_BRANCH_CAPACITY;

    *bank = new_bank;
}

/**
 * @brief Adds a new Branch to the Bank
 * @param Bank bank pointer
 * @param const char branch_name pointer
 * @return void
 */
void add_branch(Bank *bank, const char *branch_name)
{
    Branch *new_branch = (Branch *)calloc(1, sizeof(Branch));
    obj_alloc(new_branch, BRANCH);
    new_branch->head = NULL;
    new_branch->tail = NULL;
    new_branch->next = NULL;
   
    // alloc and copy the branch_name
    size_t name_len = strlen(branch_name) + 1;
    new_branch->branch_id = (char *)malloc(sizeof(char) * name_len);
    
    if(new_branch->branch_id == NULL)
    {
        fprintf(stderr, "Could not allocate space for the new branch\n");
        exit(EXIT_FAILURE);
    }

    strncpy(new_branch->branch_id, branch_name, name_len);
    new_branch->branch_id[name_len - 1] = '\0';

    // hashing and adding the new branch 
    uint8_t idx = hash_function(new_branch->branch_id, bank->capacity);
    
    // check if the branch at idx already exist 
    if(bank->branches[idx] != NULL)
    {
        Branch *current = bank->branches[idx];
        while(current->next != NULL) current = current->next;
        current->next = new_branch;
    }
    else
    {
        bank->branches[idx] = new_branch;
    }

    bank->branch_count += 1;
}

/**
 * @brief Finds a branch by a its id 
 * @param Bank bank pointer
 * @param const char branch_id pointer
 * @return void pointer 
 */
void *find(Bank *bank, const char *branch_id)
{
    uint8_t idx = hash_function(branch_id, BANK_BRANCH_CAPACITY);
   
    if(bank->branches[idx] != NULL)
    {
        if(strcmp(bank->branches[idx]->branch_id, branch_id) == 0)
        {
            return (void *)bank->branches[idx];
        }
        else
        {
            Branch *current = bank->branches[idx];
                
            while(current != NULL)
            {
                if(strcmp(current->branch_id, branch_id) == 0) return (void *)current;
                current = current->next;
            }

            // No Branches found inside the nodes.
            return NULL;
        }
    }
    else
    {
        fprintf(stderr, "Could not find any branches with id: %s\n", branch_id);
        return NULL;
    }
}

/**
 * @brief Opens a new Account (Customer included)
 * @param const char branch_id pointer
 * @param const char customer_name pointer
 * @return void
 */
void open_account(Bank *bank, const char *branch_id, const char *customer_name)
{
   // Find a branch first
   Branch *select_branch = (Branch *)find(bank, branch_id);
    
   if(select_branch == NULL) 
   {
       fprintf(stderr, "Could not find branch id: %s\n", branch_id);
       exit(EXIT_FAILURE);
   }
   
   // Allocate space for the new Account
   Account *account = (Account *)calloc(1, sizeof(Account));

   if(account == NULL)
   {
       fprintf(stderr, "Could not allocate space for the new account\n");
       exit(EXIT_FAILURE);
   }

   // Allocate space for the new Customer
   Customer *new_customer = (Customer *)calloc(1, sizeof(Customer));

   if(new_customer == NULL)
   {
       fprintf(stderr, "Could not allocate space for a new customer\n");
       exit(EXIT_FAILURE);
   }

   size_t name_length = strlen(customer_name) + 1;
   new_customer->fullname = (char *)malloc(sizeof(char) * name_length);

   if(new_customer->fullname == NULL)
   {
       fprintf(stderr, "Could not allocate space for a new customer's name\n");
       exit(EXIT_FAILURE);
   }

   strncpy(new_customer->fullname, customer_name, name_length);
   new_customer->fullname[name_length - 1] = '\0';

   // Copy the new Customer into the account 
   account->customer = new_customer;
   account->next = NULL;
   account->balance = 0;
   account->info = CUS_INFO_DEF_MASK;

   if(select_branch->head == NULL)
   {
       account->id = 1;
       select_branch->head = account;
       select_branch->tail = account;
   }
   else
   {
       account->id = select_branch->tail->id + 1;
       select_branch->tail->next = account;
       select_branch->tail = account;
   }
}


// UI-HELPERS  -----------------------------------------------

/**
 * @brief Helper callback used to prints the given Bank's branch
 * @param Branch branch pointer
 * @return void
 */
void print_branch(Branch *branch)
{
    Branch *current = branch;
    uint8_t count = 0;

    while(current != NULL)
    {
        printf("[%d] Branch's ID = %s\n", count, current->branch_id);
        
        if(current->head != NULL)
        {
            printf("--- %s Branch's Customers ---\n", current->branch_id);
            Account *current_account = current->head;

            while(current_account != NULL)
            {
                printf("Customer: %s\n", current_account->customer->fullname); 
                printf("Current Balance: %d\n", current_account->balance);
                current_account = current_account->next;
            }
            
            printf("------------------------------\n");
        }

        current = current->next;
        count++;
    }
}

/**
 * @brief Prints out all the branches associated 
 * with the Bank 
 * @param Bank bank pointer
 * @return void
 */
void show(Bank *bank, void(*on_found)(Branch *branch))
{
    if(bank->capacity == 0) return;

    for(uint8_t i = 0; i < bank->capacity; i++)
    {
        if(bank->branches[i] != NULL)
        {
            Branch *current = bank->branches[i];
            on_found(current);
        }
        else 
        {
            printf("[%d] index is empty for now\n", i);
        }
        
        printf("----------------------------\n");
    }
}

int main(void)
{
    Bank *bank = NULL;
    // Init a new Bank
    init_bank(&bank);
    // Add Branches
    add_branch(bank, "USA");
    add_branch(bank, "ITA");
    add_branch(bank, "NED");
    add_branch(bank, "ESP");
    add_branch(bank, "GER");
    add_branch(bank, "JAP");
    // Open new bank account 
    open_account(bank, "GER", "Valerio DAlessio");
    open_account(bank, "GER", "Leonor Fernandez");
    // Prints all stored Branches
    show(bank, print_branch); 
    return 0;
}
