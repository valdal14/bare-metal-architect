#include <stdio.h>

struct NetworkPacket
{
    int id;
    int value;
};

/**
 * @brief Simulates receiving and injecting data into a raw network buffer.
 * @param raw_buffer Pointer to an 8-byte character array.
 * @param parse Function pointer to the callback that will decode the buffer.
 * @return void
 */
void struct_overlay_example(char *raw_buffer, void (* parse)(char *))
{
    // Cast the base address to an int pointer, then write 4 bytes
    *(int *)(raw_buffer) = 1024;

    // Walk forward 4 bytes, cast that new address to an int pointer, and write
    *(int *)(raw_buffer + 4) = 8899;

    parse(raw_buffer);
}

/**
 * @brief Parses a raw byte stream into a NetworkPacket struct using zero-copy casting.
 * @param data A pointer to the raw byte buffer.
 * @return void
 */
void parse(char *data)
{
    struct NetworkPacket packet = *(struct NetworkPacket *)data;
    printf("Packet's id    = %d\n", packet.id);
    printf("Packet's value = %d\n", packet.value);
}

int main(void)
{
    char stream_buffer[8];

    struct_overlay_example(stream_buffer, parse);
    return 0;
}
