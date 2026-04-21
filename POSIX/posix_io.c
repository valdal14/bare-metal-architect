#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

struct Payload
{
    int id;
    double value;
};

/**
 * @brief Validates a POSIX file descriptor and exits on failure.
 * @param fd The file descriptor returned by open().
 * @return void
 */
void check_fd(int fd)
{
    if(fd == -1)
    {
        fprintf(stderr, "Access Denied or File Not Found\n");
        exit(1);
    }
}

/**
 * @brief Writes a Payload struct directly to a binary file using POSIX calls.
 * @param completion A callback function to execute after the write finishes.
 * @return void
 */
void writer(void (*completion)(void))
{
    printf("Writing to disk...\n");
    struct Payload payload;
    payload.id = 14;
    payload.value = 47.7;
    
    // Open for writing, create if missing, truncate if exists, with 0644 permissions
    int fd = open("test.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    check_fd(fd);

    // Using ssize_t for POSIX compliance
    ssize_t bytes_written = write(fd, &payload, sizeof(struct Payload));
    printf("Successfully wrote %zd bytes.\n", bytes_written);
    
    close(fd);
    
    // Trigger the callback
    completion();
}

/**
 * @brief Reads a Payload struct from a binary file and prints its contents.
 * @return void
 */
void reader(void)
{
    printf("\nReading from disk...\n");
    struct Payload payload;
    
    // Open strictly for reading
    int fd = open("test.bin", O_RDONLY);
    check_fd(fd);

    // Using ssize_t for POSIX compliance
    ssize_t bytes_read = read(fd, &payload, sizeof(struct Payload));
    
    printf("Successfully read %zd bytes.\n", bytes_read);
    
    // Prove the data survived the round trip to the hard drive
    printf("--- Retrieved Data ---\n");
    printf("Payload ID: %d\n", payload.id);
    printf("Payload Value: %f\n", payload.value);

    close(fd);
}

int main(void)
{
    writer(reader);
    return 0;
}
