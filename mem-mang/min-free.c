#include <stdio.h>

int main(void)
{
    unsigned char block1[8];
    unsigned char block2[8];
    unsigned char block3[8];
    
    // ==========================================
    // PHASE 1: WRITING (Putting the map in the sand)
    // ==========================================
    
    void **lens1 = (void **)block1;
    lens1[0] = block2; 
    
    void **lens2 = (void **)block2;
    lens2[0] = block3;
    
    void **lens3 = (void **)block3;
    lens3[0] = NULL;
    
    // ==========================================
    // PHASE 2: READING (Following the map)
    // ==========================================
    
    // We start our tracker at block 1
    void *tracker = block1;
    
    while(tracker != NULL)
    {
        printf("Tracker is currently at: %p\n", tracker);
        
        // Reading requires the EXACT SAME LENS as writing!
        
        // 1. Create a reading lens and point it at the sand we are standing on.
        void **reading_lens = (void **)tracker;
        
        // 2. Look through the lens, read the 8-byte address buried there, 
        // and physically move our tracker to that new address.
        tracker = reading_lens[0];
    }
    
    return 0;
}
