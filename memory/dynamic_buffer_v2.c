#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fill_buffer(char *buffer)
{
    char *data[5] = {"H","e","l","l","o"};
    int size = sizeof(data) / sizeof(data[0]);

    for(int i = 0; i < size; i++)
       buffer[i] = *data[i];
}

/**
 * @brief Simulates allocating a dynamic buffer for an incoming network payload.
 * @param payload_size The size of the payload in bytes.
 * @return void
 */
void process_dynamic_payload(int payload_size) {
    printf("Incoming payload detected. Size: %d bytes.\n", payload_size);
    
    // allocate 'payload_size' bytes of memory.,:
	char *buffer = (char *)malloc(payload_size * sizeof(char)); 
    // Check if malloc failed.
    if(buffer == NULL) 
    {
        fprintf(stderr, "Error allocating memory\n");
    }
    
    printf("Memory successfully allocated on the Heap at address: %p\n", (void *)buffer);
    
    // simulate copying network bytes into our new buffer.
    //memset(buffer, 'A', payload_size);
    fill_buffer(buffer);
    
    // manually set the very last byte to the null terminator '\0' 
    // so it can safely print it as a string in C.
    buffer[payload_size - 1] = '\0';
    
    printf("Buffer contents: %s\n", buffer);
    
    free(buffer);
    
    printf("Memory freed. No leaks!\n");
}

int main() {
    // Simulate a small payload
    process_dynamic_payload(4);
    
    printf("\n");
    
    // Simulate a larger payload (e.g., from a different client)
    process_dynamic_payload(128);
    
    return 0;
}
