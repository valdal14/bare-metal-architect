#include <stdio.h>
#include <stdlib.h>

struct LegendsCard
{
    char *name;
    int magicka_cost;
    // Linked list of cards within the deck
    struct LegendsCard *next; 
};

/**
 * @brief Init the deck with 3 cards
 * @return struct LegendsCard pointer
 */
struct LegendsCard *init_deck()
{
    struct LegendsCard *head = (struct LegendsCard *)calloc(1, sizeof(struct LegendsCard));
    if(head == NULL) exit(1);
    head->name = "Almalexia";
    head->magicka_cost = 10;
    head->next = NULL;

    struct LegendsCard *card1 = (struct LegendsCard *)calloc(1, sizeof(struct LegendsCard));
    if(card1 == NULL) exit(1);
    card1->name = "Vivec";
    card1->magicka_cost = 5;
    card1->next = NULL;

    
    struct LegendsCard *card2 = (struct LegendsCard *)calloc(1, sizeof(struct LegendsCard));
    if(card2 == NULL) exit(1);
    card2->name = "Barrow Stalcker";
    card2->magicka_cost = 2;
    card2->next = NULL;

    head->next = card1;
    card1->next = card2;

    return head;
}

void bubble_sort_cards(struct LegendsCard *list, unsigned int num_cards)
{
    int swapped = 0;
    int counter = 0;
    struct LegendsCard cards[num_cards];
    
    struct LegendsCard *current = list;

    printf("- Current Order\n");

    while(current != NULL)
    {
        printf("%s costs %d\n", current->name, current->magicka_cost);
        // dereferencing to copy the value
        cards[counter] = *current;
        current = current->next;
        counter++;
    }
    
    printf("-----------------\n");
    
    // Bubble Sort Logic
    do {
    
        swapped = 0; // Reset at the start of every full array sweep

        for(unsigned int i = 0; i < num_cards - 1; i++)
        {
            if(cards[i].magicka_cost > cards[i + 1].magicka_cost)
            {
                // Perform the swap
                struct LegendsCard temp = cards[i];
                cards[i] = cards[i+1];
                cards[i+1] = temp;

                swapped = 1;
            }
        }

    } while (swapped == 1); // If we swapped anything, we have to check the whole array again!


    printf("- Sorted Order\n");
    for(unsigned int i = 0; i < num_cards; i++)
        printf("%s costs %d\n", cards[i].name, cards[i].magicka_cost);
}



int main(void)
{
    struct LegendsCard *deck = init_deck();
    bubble_sort_cards(deck, 3);

    return 0;
}
