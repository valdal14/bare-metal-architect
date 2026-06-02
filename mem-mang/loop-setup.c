#include <stdio.h>

int main(void)
{
    // 24 bytes of sand (3 boxes)
    unsigned char pool[24];
    
    int counter = 0;

    while(counter < 3)
    {
        // 1. The box we are standing in front of right now
        unsigned char *current_box = pool + (8 * counter);
        
        // 2. What do we write on the piece of paper?
        void *paper;
        if (counter == 2) 
        {
            // If we are at the last box, write STOP (NULL)
            paper = NULL; 
        } 
        else 
        {
            // Otherwise, write the address of the NEXT box
            paper = pool + (8 * (counter + 1)); 
        }

        // 3. Drop the paper into the box.
        // Because the paper is a memory address, we must use the 2-star lens.
        void **lens = (void **)current_box;
        lens[0] = paper; 

        counter++;
    }

    // ==========================================
    // TEST THE HIGHWAY
    // ==========================================
    
    // Start at the first box
    void *tracker = pool; 
    
    printf("\nDriving the highway...\n");
    
    // Drive until we hit the STOP sign (NULL)
    while(tracker != NULL)
    {
        printf("Arrived at box: %p\n", tracker);
        
        // Put on the 2-star glasses to read the paper inside the box
        void **reading_lens = (void **)tracker;
        
        // Make the tracker jump to the address written on the paper
        tracker = reading_lens[0]; 
    }

    return 0;
}
