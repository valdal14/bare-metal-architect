#include <stdio.h>

/**
 * @brief Reads and prints a contiguous memory block using standard array indexing.
 * * @param buffer Pointer to the first character of the memory block.
 * @param length The total number of bytes (characters) to read.
 * @return void
 */
void read_with_index(char *buffer, int len)
{
    printf("Reading via Array Indexing:\n");

    for(int i = 0; i < len; i++)
        printf("buffer %d holds %c\n", i, buffer[i]);
}

/**
 * @brief Reads and prints a contiguous memory block using raw pointer arithmetic.
 * * @param buffer Pointer to the first character of the memory block.
 * @param length The total number of bytes (characters) to read.
 * @return void
 */
void read_with_pointers(char *buffer, int len)
{
    printf("Reading via Pointer Arithmetic:\n");

    for(int i = 0; i < len; i++)
        printf("buffer %d holds %c\n", i, *(buffer + i));
}

int main(void)
{
    // We simulate a raw payload caught in a memory buffer.
    char xml_payload[] = "<stream><data>42</data></stream>";

    // In C, arrays don't know their own length. We must calculate it.
    // sizeof(xml_payload) gives the total bytes. sizeof(xml_payload[0]) gives the size of one element.
    // We subtract 1 at the end to ignore the invisible '\0' (null terminator) that C adds to strings.
    int payload_length = (sizeof(xml_payload) / sizeof(xml_payload[0])) - 1;

    printf("Buffer allocated. Payload length: %d bytes.\n\n", payload_length);
    read_with_index(xml_payload, payload_length);
    read_with_pointers(xml_payload, payload_length);
    return 0;
}
