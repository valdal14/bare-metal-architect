#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/uio.h>

#define PORT 5001
#define QUEUE_SIZE 2
#define BUFFER_SIZE 1024
/**
 * @brief Creates a new socket 
 * @return int 
 */
int create_socket(void)
{
    const struct protoent *protoent = getprotoent();
    int server_sock = socket(AF_INET, SOCK_STREAM, protoent->p_proto); 
    
    if(server_sock == -1)
    {
        fprintf(stderr, "Error: Opening the server socket using %s protocol\n", protoent->p_name);
        exit(EXIT_FAILURE);
    }

    return server_sock;
}

/**
 * @brief Creates a new socket address
 * @param uint8_t port_num
 * @return struct sockaddr_in
 */
struct sockaddr_in create_socket_address(uint16_t port_num)
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port_num);

    return address;
}

/**
 * @brief Binds the socket to the socket address
 * @param int server_socket
 * @param sockaddr_in address pointer
 * @return void
 */
void bind_socket(int server_socket, struct sockaddr_in *address)
{
    int bind_result = bind(server_socket, (struct sockaddr *)address, sizeof(*address));

    if(bind_result == -1)
    {
        fprintf(stderr, "Error: Binding the address with the socket\n");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Socket successfully bound to port %d\n", PORT);
}

/**
 * @brief Listens passively for incoming connections
 * @param int server_socket: The socket file id 
 * @param int backlog: The socket queue 
 * @return void
 */
void start_listening(int server_socket, int backlog)
{
    // The listen() call applies only to sockets of type SOCK_STREAM
    int listen_result = listen(server_socket, backlog);
    
    if(listen_result == -1)
    {
        fprintf(stderr, "Error: Server Socket could not change its state to listening\n");
        // TODO
        // ECONNREFUSED: if the underlying protocol supports retransmission, the request may be ignored so that retries may succeed
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server Socket started listening\n");
}

/**
 * @brief Accepts an incoming connection by creating a new socket FD. 
 * @param int server_socket
 * @param struct sockaddr restrict address pointer
 * @param socklen_t restrict address_len pointer
 * @return int: The client socket file descriptor value
 */
int accept_connection(int server_socket, struct sockaddr *restrict address)
{
    socklen_t address_len = sizeof(address);
    int accept_result = accept(server_socket, address, &address_len);

    if(accept_result == -1)
    {
        fprintf(stderr, "Error: Could not accept incoming connection\n");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    return accept_result;
}

/**
 * @brief Reads the message sends by the client
 * @param int client_socket 
 * @param char buffer pointer
 * @param ssize_t buffer_size
 * @return void
 */
void read_msn(int client_socket, char *buffer, ssize_t buffer_size)
{

    ssize_t bytes_read = read(client_socket, buffer, buffer_size - 1);
    
    if (bytes_read == -1)
    {
        fprintf(stderr, "Error: Reading from file descriptor\n");
        exit(EXIT_FAILURE);
    }

    if (bytes_read == 0)
    {
        fprintf(stderr, "Error: Connection closed or End of File reached.\n");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_read] = '\0';

    printf("Received %zd bytes: %s\n", bytes_read, buffer);
}

int main(void)
{
    char buffer[BUFFER_SIZE];

    int server_sock = create_socket();
    struct sockaddr_in address = create_socket_address(PORT);
    bind_socket(server_sock, &address);
    start_listening(server_sock, QUEUE_SIZE);
    int client_socket = accept_connection(server_sock, (struct sockaddr *)&address);
    // read the client socket
    read_msn(client_socket, buffer, BUFFER_SIZE);

    close(server_sock);
    return 0;
}
