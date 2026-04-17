#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IP_ADDRESS_SIZE 16

struct ServerConfig
{
    int port;
    char ip_address[IP_ADDRESS_SIZE];
};

struct ServerConfig* init()
{
    struct ServerConfig *server_config = (struct ServerConfig*)malloc(sizeof(struct ServerConfig));
    if(server_config == NULL)
    {
        fprintf(stderr, "Cannot instanciate a the struct");
        exit(1);
    }

    return server_config;
}

void prints(struct ServerConfig *config)
{
    printf("port       = %d\n", config->port);
    printf("ip address = %s\n", config->ip_address);
    printf("----------------------------------\n");
}

void set_server_b_config(int port, char *ip)
{
    struct ServerConfig *config = init();

    for(int i = 0; i < IP_ADDRESS_SIZE; i++)
       config->ip_address[i] = *(ip + i);

    config->port = port;
    
    prints(config);

    free(config);
}

void free_servers(struct ServerConfig* servers[], int size)
{
    for(int i = 0; i < size; i++)
    {
        printf("Deallocating server on PORT = %d \n", servers[i]->port);
        free(servers[i]);
    }
}

int main(void)
{
    // using strcpy for copying the string
    struct ServerConfig *config_A = init();    
    config_A->port = 80;
    strcpy(config_A->ip_address, "192.168.1.1"); 
    prints(config_A);
    
    // using iteration to copy the ip address
    char ip[IP_ADDRESS_SIZE] = "192.168.19.21";
    int port = 81;

    set_server_b_config(port, ip);
    
    // using Struct Assigniment to copy 
    struct ServerConfig *config_B = init();
    *config_B = *config_A;
    config_B->port = 8080;
    prints(config_B);

    // using memcpy to copy 
    struct ServerConfig *config_C = init();
    memcpy(config_C, config_A, sizeof(struct ServerConfig));
    // changing the value
    config_C->port = 443;
    prints(config_C);
    
    // The alias trap...
    // Correct way to alias: Just point, do not allocate!
    struct ServerConfig *alias_config = config_A;
    prints(alias_config);
    // changing the alias affects the original configuration
    alias_config->port = 5555;
    prints(alias_config);
    prints(config_A);

    // array of pointers
    struct ServerConfig *servers[3] = {config_A, config_B, config_C};
    free_servers(servers, 3);

    return 0;
}
