#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 24

struct Deck
{
    char *name;
    unsigned short cost;
    struct Deck *next;
    struct Deck *prev;
};

/**
 * @brief Factory method 
 * @return struct Deck pointer
 */
struct Deck *init_deck(void)
{
    struct Deck *deck = (struct Deck *)calloc(1, sizeof(struct Deck));
    if(deck == NULL) exit(1);
    // add a default card to prepare the Deck to show how insertion sort works
    deck->name = "Almalexia";
    deck->cost = 10;
    deck->next = NULL;
    deck->prev = NULL;

    return deck;
}

/**
 * @brief Add a new card to the deck
 * @param struct Deck deck pointer of pointers
 * @param unsigned short cost
 * @return void
 */
void add_card(struct Deck **deck_ref, char *name, unsigned short cost)
{
    int str_length = strlen(name) + 1;
    if(str_length > MAX_LENGTH) {
        fprintf(stderr, "Card's name must be at most %d chars length\n", MAX_LENGTH);
        exit(1);
    }

    // 1. Create the new node
    struct Deck *new_card = (struct Deck *)calloc(1, sizeof(struct Deck));
    if(new_card == NULL) exit(1);
    
    new_card->name = (char *)malloc(str_length);
    strncpy(new_card->name, name, str_length);
    new_card->name[str_length - 1] = '\0';
    
    new_card->cost = cost;
    new_card->next = NULL;
    new_card->prev = NULL;

    // ==========================================
    // PHASE 1: THE SEARCH
    // ==========================================
    struct Deck *current = *deck_ref;
    struct Deck *prev_node = NULL;

    // Walk the list until we find a card MORE expensive than our new card
    while(current != NULL)
    {
        if(current->cost > cost) {
            break; // We found the insertion point! Stop searching.
        }
        prev_node = current;
        current = current->next;
    }

    // ==========================================
    // PHASE 2: THE WIRING (The Three Realities)
    // ==========================================

    if (prev_node == NULL) 
    {
        // Use-Case  1: Insert at Head (The list was empty, OR new_card is the cheapest)
        new_card->next = *deck_ref; // Point forward to the old head
        
        if (*deck_ref != NULL) {
            (*deck_ref)->prev = new_card; // Old head points back to new card
        }
        
        *deck_ref = new_card; // OVERWRITE MAIN'S STICKY NOTE!
    } 
    else if (current != NULL) 
    {
        // Use-Case  2: Insert in the Middle (between prev_node and current)
        new_card->next = current;
        new_card->prev = prev_node;
        
        prev_node->next = new_card;
        current->prev = new_card;
    } 
    else 
    {
        // Use-Case  3: Insert at Tail (new_card is the most expensive)
        new_card->prev = prev_node;
        prev_node->next = new_card;
    }
}

/**
 * @brief Prints the deck 
 * @param struct Deck deck pointer
 * @return void
 */
void print_deck(struct Deck *deck)
{
    struct Deck *current = deck;
    
    while(current != NULL)
    {
        printf("%s cost = %d\n", current->name, current->cost);
        current = current->next;
    }
}

/**
 * @brief Recursively clean the allocated memory
 * @param struct Deck deck pointer
 * @return void
 */
void free_memory(struct Deck *deck)
{
    if(deck == NULL) return; 

    // DIVE TO THE BOTTOM FIRST
    free_memory(deck->next);

    // FREE ON THE WAY BACK UP
    // We only reach these lines AFTER the nodes ahead of us have been destroyed.
    free(deck->name);
    free(deck); 
}

int main(void)
{
    struct Deck *deck = init_deck();
    add_card(&deck, "Vivec", 5);
    add_card(&deck, "Barrow Stalker", 2);
    print_deck(deck);
    free_memory(deck);
    return 0;
}
