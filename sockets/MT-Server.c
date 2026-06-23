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
#define QUEUE_SIZE 5 
#define BUFFER_SIZE 1024

// Global Mutex Lock
pthread_mutex_t logger_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct Message
{
    char buffer[BUFFER_SIZE];
    struct Message *next; 
    char *msn;
    int client_socket;
} Message;

typedef struct
{
    struct Message *head;
    struct Message *tail;
    int server_socket;
    uint8_t total_connection;
} ServerLogger;

typedef struct {
    int client_socket;
    ServerLogger *logger;
} ThreadArgs;

/**
 * @brief Instanciate a new ServerLogger to monitor the incoming messages 
 * from the client sockets 
 * @param ServerLogger logger double pointer
 * @param int server_socket
 * @return void
 */
void init_server_logger(ServerLogger **logger, int server_socket)
{
    ServerLogger *server_logger = (ServerLogger *)calloc(1, sizeof(ServerLogger));

    if(server_logger == NULL)
    {
        fprintf(stderr, "Error: Could not instanciate a ServerLogger\n");
        exit(EXIT_FAILURE);
    }

    server_logger->head = NULL;
    server_logger->tail = NULL;
    server_logger->total_connection = 0;
    server_logger->server_socket = server_socket;
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
    size_t msn_size = strlen(msn);
    Message *message = (Message *)malloc(sizeof(Message));
    message->msn = (char *)malloc(sizeof(char) * msn_size);
    
    if(message == NULL || message->msn == NULL)
    {
        fprintf(stderr, "Error: Could not allocate the space for the new message. The client message will be lost\n");
        return;
    }

    strncpy(message->msn, msn, msn_size);
    message->msn[msn_size + 1] = '\0';
    message->client_socket = client_socket;
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
 * @brief Shows a recap of the client session and clean up the memory 
 * @param ServerLogger logger pointer 
 * @return void
 */
void *shutdown_server(ServerLogger *logger)
{
    Message *current = logger->head;
    Message *next_message = NULL;

    while(current != NULL)
    {
        printf("----------------------------------------\n");
        printf("Client ID: %d\n", current->client_socket);
        printf("Message  : %s\n", current->msn);
        next_message = current->next;
        free(current);
        current = next_message;
        printf("----------------------------------------\n");
    }

    close(logger->server_socket);
    free(logger);
    logger = NULL;
    return NULL;
}

/**
 * @brief Handles the client connection
 * @param void arg pointer
 */
void *handle_client(void *arg) {
    
    ThreadArgs *my_args = (ThreadArgs *)arg;
    int my_socket = my_args->client_socket;
    ServerLogger *my_logger = my_args->logger;

    char buffer[BUFFER_SIZE];

    char *message = read_msn(my_socket, buffer, BUFFER_SIZE);

    pthread_mutex_lock(&logger_lock);
    log_message(my_logger, message, my_socket);
    pthread_mutex_unlock(&logger_lock);

    close(my_socket);
    free(my_args);

    return NULL;
}

int main(void)
{
    //char buffer[BUFFER_SIZE];
    ServerLogger *logger = NULL;
    
    int server_socket = create_socket();
    
    init_server_logger(&logger, server_socket);
    
    struct sockaddr_in address = create_socket_address(PORT);
    bind_socket(server_socket, &address);
    
    start_listening(server_socket, QUEUE_SIZE);
    
    int current_connections = 0;

	while(current_connections < QUEUE_SIZE) {
	    int client_sock = accept_connection(server_socket, (struct sockaddr *)&address);
	    current_connections++;

	    ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
	    args->client_socket = client_sock;
	    args->logger = logger;

	    pthread_t thread_id;
	    pthread_create(&thread_id, NULL, handle_client, args);
	    pthread_detach(thread_id);
	}

	shutdown_server(logger);
	
    return 0;
}
