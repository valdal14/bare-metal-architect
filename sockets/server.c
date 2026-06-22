#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define PORT 5001

/**
 * @brief Creates a new socket 
 * @return int 
 */
int create_socket(void)
{
    const struct protoent *protoent = getprotoent();
    int server_sock = socket(AF_INET, SOCK_STREAM, protoent->p_proto); 
    printf("%d\n", server_sock);
    
    if(server_sock == -1)
    {
        fprintf(stderr, "Error opening the server socket using %s protocol\n", protoent->p_name);
        exit(EXIT_FAILURE);
    }

    return server_sock;
}

/**
 * @brief Creates a new socket address
 * @return struct sockaddr_in
 */
struct sockaddr_in create_socket_address(void)
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

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

    printf("%d\n", bind_result);
    if(bind_result == -1)
    {
        fprintf(stderr, "Error binding the address with the socket\n");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Socket successfully bound to port %d\n", PORT);
}


int main(void)
{
    int server_sock = create_socket();
    struct sockaddr_in address = create_socket_address();
    bind_socket(server_sock, &address);

    close(server_sock);

    return 0;
}
