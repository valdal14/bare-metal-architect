#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define VIEWS_COUNT 4

/* File-scoped static array. Kept private to this file. */
static const char *const DB_VIEWS[VIEWS_COUNT] = {
    "users_view",
    "orders_view",
    "inventory_view",
    "logs_view"
};

/**
 * @brief Hashes a string key into a valid array index.
 * @param key The string to hash.
 * @return uint8_t The calculated index.
 */
uint8_t hash_function(const char *key) {
    int hash = 0;
    
    while (*key != '\0') 
    {
        hash = (hash + *key) % VIEWS_COUNT;
        key++;
    }
    
    return hash;
}

/**
 * @brief The purpose of this function is to quicly validate 
 * if a given view's name key is found in the DB_VIEWS. It does
 * it by hashing the key and check whether the view is present.
 * If it is, returns true. if there is a collision it will 
 * perform a O(n) runtime search and scan the entire DB_VIEWS
 * looking for it. If it is found returns true otherwise the 
 * function will return false. 
 *
 * Best  case runtime O(1)
 * Worst case runtime O(n)
 *
 * @param const char key pointer 
 * @return bool
 */
bool verify_view_by(const char *key)
{
    uint8_t view_idx = hash_function(key);
    printf("idx = %d\n", view_idx);
    const char *view = DB_VIEWS[view_idx];
   
    if(strcmp(key, view) == 0)
        return true;
    else
        for(uint8_t i = 0; i < VIEWS_COUNT; i++)
            if(strcmp(key, DB_VIEWS[i]) == 0) return true;

    return false;
}

int main(void)
{
    bool is_valid = verify_view_by("logs_view");
    is_valid == true ? printf("found it\n") : printf("Not found\n");
    return 0;
}
