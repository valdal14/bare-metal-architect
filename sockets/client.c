#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/uio.h>

#define SERVER_PORT 5001

int main(void)
{
    char msn[22] = "OXFORD_NODE_01_ONLINE";
    
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket == -1)
    {
        fprintf(stderr, "Error: Could not create socket\n");
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    // Client should connect to a specific IP. LOOPBACK targets localhost (127.0.0.1)
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
    address.sin_port = htons(SERVER_PORT);
    socklen_t address_len = sizeof(address);

    int con_res = connect(client_socket, (struct sockaddr *)&address, address_len);
    
    if(con_res == -1)
    {
        fprintf(stderr, "Error: Could not establish a connection with the server\n");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Write to the actual socket file descriptor
    ssize_t write_res = write(client_socket, msn, sizeof(msn)); 
    
    if(write_res == -1)
    {
        fprintf(stderr, "Error: Failed to write data to the server\n");
    }
    else
    {
        printf("Successfully wrote %zd bytes to server.\n", write_res);
    }

    close(client_socket);
    return 0;
}
