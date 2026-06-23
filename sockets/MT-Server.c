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
#include <pthread.h>

#define PORT 5001
#define QUEUE_SIZE 2
#define BUFFER_SIZE 1024

// Global Mutex Lock
pthread_mutex_t logger_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct Message
{
    char *msn;
    struct Message *next; 
} Message;

typedef struct
{
    int client_socket;
    struct Message *head;
    struct Message *tail;
    uint8_t total_connection;
} ServerLogger;

/**
 * @brief Instanciate a new ServerLogger to monitor the incoming messages 
 * from the client sockets 
 * @param ServerLogger logger double pointer
 * @return void
 */
void init_server_logger(ServerLogger **logger)
{
    ServerLogger *server_logger = (ServerLogger *)calloc(1, sizeof(ServerLogger));

    if(server_logger == NULL)
    {
        fprintf(stderr, "Error: Could not instanciate a ServerLogger\n");
        exit(EXIT_FAILURE);
    }

    server_logger->client_socket = 0;
    server_logger->head = NULL;
    server_logger->tail = NULL;
    server_logger->total_connection = 0;

    *logger = server_logger;
}

/**
 * @brief Logs (registers) a new incoming client connection and 
 * its incoming message 
 * @param ServerLogger logger pointer
 * @param char msn pointer
 * @param int client_socket
 * @return void
 */
void log_message(ServerLogger *logger, char *msn, int client_socket)
{
    size_t msn_size = strlen(msn) + 1;
    Message *message = (Message *)malloc(sizeof(Message));
    message->msn = (char *)malloc(sizeof(char) * msn_size);
    
    if(message == NULL || message->msn == NULL)
    {
        fprintf(stderr, "Error: Could not allocate the space for the new message. The client message will be lost\n");
        return;
    }

    strncpy(message->msn, msn, msn_size);
    message->msn[msn_size - 1] = '\0';
    message->next = NULL;

    if(logger->head == NULL)
    {
        logger->head = message;
        logger->tail = message;
    }
    else
    {
        logger->tail->next = message;
        logger->tail = message;
    }

    logger->client_socket = client_socket;
    logger->total_connection += 1;
}

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
    socklen_t address_len = sizeof(struct sockaddr);
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
 * @return char pointer
 */
char *read_msn(int client_socket, char *buffer, ssize_t buffer_size)
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
    return buffer;
}

/**
 * @brief Handles the client connection
 * @param void arg pointer
 */
void *handle_client(void *arg) 
{
    ServerLogger *client_msn = (ServerLogger *)arg;
    
    if(client_msn == NULL)
    {
        fprintf(stderr, "Error: Could not cast the request to the correct type\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Handling new client socket with id: %d\n", client_msn->client_socket);
    printf("Client Message: %s\n", client_msn->tail->msn);
    printf("Session: %d\n", client_msn->total_connection);
    printf("Client Request Registered Successfully\n");
    close(client_msn->client_socket);
    return NULL;
}

/**
 * @brief Processes the client request by accepting the request, and spawing a new thread 
 * to lock the resource to avoid race conditions on the ServerLogger data
 * @param int server_socket
 * @param struct sockaddr_in address pointer
 * @param ServerLogger logger pointer
 * @param char buffer pointer
 * @return void
 */
void process_client_request(int server_socket, struct sockaddr_in *address, ServerLogger *logger, char *buffer)
{
    int *client_socket = (int *)calloc(1, sizeof(int));
        
    if(client_socket == NULL)
    {
        fprintf(stderr, "Error: Could not allocate space for the new client socket\n");
        exit(EXIT_FAILURE);
    }
    
    // accept the connection
    *client_socket = accept_connection(server_socket, (struct sockaddr *)&address);
    
    // read the buffer to extract the client message 
    char *message = read_msn(*client_socket, buffer, BUFFER_SIZE);

    // lock and record the new client connection message
    pthread_mutex_lock(&logger_lock);
    log_message(logger, message, *client_socket);
    pthread_mutex_unlock(&logger_lock);
    // Spawn the thread, passing the memory address of the client_socket
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, handle_client, logger);
}

int main(void)
{
    char buffer[BUFFER_SIZE];
    ServerLogger *logger = NULL;
    init_server_logger(&logger);

    int server_sock = create_socket();
    struct sockaddr_in address = create_socket_address(PORT);
    bind_socket(server_sock, &address);
    start_listening(server_sock, QUEUE_SIZE);

    while(1)
    {
        process_client_request(server_sock, &address, logger, buffer);
    }

    close(server_sock);
    return 0;
}
