#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HT 0
#define KVP 1
#define KEY 2

struct KeyValuePair 
{
    char *key;
    int value;
    struct KeyValuePair *next;
};

struct HashTable 
{
    struct KeyValuePair **buckets;
    int capacity;
};

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
 * @brief Checks the pointer allocation
 * @param void obj pointer
 * @param int type The type of the object to verify
 * @return void
 */
void check_alloc(void *obj, int type)
{
    switch(type)
    {
        case 0:
            if((struct HashTable *)obj == NULL)
            {
                fprintf(stderr, "Cannot allocate space for the HashTable struct\n");
                exit(1);
            }
            break;
        case 1:
            if((struct KeyValuePair *)obj == NULL)
            {
                fprintf(stderr, "Cannot allocate space for the KVP bucket\n");
                exit(1);
            }
            break;
        case 2:
            if((char *)obj == NULL)
            {
                fprintf(stderr, "Cannot allocate space for the key\n");
                exit(1);
            }
            break;
        default:
            printf("3\n");
            fprintf(stderr, "Type is not supported \n");
            break;
    }
}

/**
 * @brief Factory method that creates the HT instance
 * @param int capacity The capacity of the bucket
 * @return struct HashTable pointer
 */
struct HashTable* table_create(int capacity)
{
    struct HashTable *ht = (struct HashTable *)calloc(1, sizeof(struct HashTable));
    check_alloc(ht, HT);
    
    ht->buckets = (struct KeyValuePair **)calloc(capacity, sizeof(struct KeyValuePair *));
    check_alloc(ht->buckets, KVP);

    ht->capacity = capacity;
    return ht;
}

/**
 * @brief Add a new KVP object into the HT
 * @param struct HashTable table pointer
 * @param const char key pointer to a read only key
 * @param int value The value we want to store
 * @return void
 */
void table_insert(struct HashTable *table, const char *key, int value)
{
   struct KeyValuePair *kvp = (struct KeyValuePair *)malloc(sizeof(struct KeyValuePair));
   check_alloc(kvp, KVP);
    
   int key_size = strlen(key) + 1;
   // hash the key
   int index = hash_function(key, table->capacity);
   
   // alloc space for the key
   kvp->key = (char *)malloc(key_size);
   check_alloc(kvp->key, KEY);
   // assign value & index
   strcpy(kvp->key, key);
   kvp->value = value;
   
   // assign it to the HT
   if(table->buckets[index] == NULL)
   {
       kvp->next = NULL;
       table->buckets[index] = kvp;
   }
   else
   {
       // assign to the next the head
       kvp->next = table->buckets[index];
       // se the head as the new KVP object
       table->buckets[index] = kvp;
   }
}

/**
 * @brief Retrieve the value from the given key from the HT
 * @param struct HashTable pointer
 * @param const char key Read only string
 * @return int The value
 */
int table_get(struct HashTable *table, const char *key)
{
    int index = hash_function(key, table->capacity);
    
    if(table->buckets[index] == NULL)
    {
        fprintf(stderr, "Could not find any value for key = %s \n", key);
        exit(1);
    }
    
    struct KeyValuePair *current = table->buckets[index];
    // If it's NULL immediately, the loop skips.
    while(current != NULL)
    { 
        if(strcmp(current->key, key) == 0) return current->value;
        current = current->next;
    }

    // Safe fallback if the key doesn't exist
    return -1;
}

/**
 * @brief Remove allocated memory
 * @param struct HastTable pointer
 * @return void
 */
void table_free(struct HashTable *table)
{
    for(int i = 0; i < table->capacity; i++)
    {
        if(table->buckets[i] != NULL)
        {
            struct KeyValuePair *current = table->buckets[i];
            struct KeyValuePair *next_node = NULL;

            while(current != NULL)
            {
                next_node = current->next;
                free(current->key);
                free(current);
                current = next_node;
            }    
        }
    }

    free(table->buckets);
    free(table);
}


int main(void)
{
    struct HashTable *ht = table_create(5);

    table_insert(ht, "Leonor", 8);
    table_insert(ht, "Val", 14);
    table_insert(ht, "Grace", 19);
    table_insert(ht, "Leonordo", 11);
    table_insert(ht, "Nash", 11);
    table_insert(ht, "Turing", 10);

    int val1 = table_get(ht, "Grace");
    printf("For key 'Grace' I found the value %d\n", val1);

    int val2 = table_get(ht, "Turing");
    printf("For key 'Turing' I found the value %d\n", val2);
   
    int val3 = table_get(ht, "Nash");
    printf("For key 'Nash' I found the value %d\n", val3);
   
    table_free(ht);
    return 0;
}
