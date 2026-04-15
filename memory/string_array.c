#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SERVERS_ALLOWED 5
#define MAX_SERVER_NAME_ALLOWED 7

void add_new_to(char** servers, int counter)
{

    for(int i = 0; i < MAX_SERVERS_ALLOWED; i++)
    {
        if(servers[i] == NULL)
        {
            printf("The servers buffer is empty\n");
            break;
        } 
        else
        {
            printf("Adding new server to the list at position %d\n", counter);
            break;
        }

    }

    servers[counter] = (char *)malloc(MAX_SERVER_NAME_ALLOWED * sizeof(char));

    if(servers[counter] == NULL)
    {
        fprintf(stderr, "Cannot allowcate space for a new server");
        exit(1);
    }
    
    strcpy(servers[counter], "Server");
    printf("Server at position %d was  added to the list\n", counter);
}

void verify_servers_alloc(char **servers)
{
    if(servers == NULL)
    {
        fprintf(stderr, "Servers list cannot be instanciated");
        exit(1);
    }
}

void verify_server_alloc(char *server)
{
    if(server == NULL)
    {  
        fprintf(stderr, "Server cannot be instanciated");
        exit(1);
    }
}

void free_mem(char **servers)
{
    // destroy the ribs
    for(int i = 0; i < MAX_SERVERS_ALLOWED; i++)
    {
        if(servers[i] != NULL)
        {
            printf("deallocating server at index: %d\n", i);
            free(servers[i]);
        }
    }

    free(servers);
}

char** allocate_server_space()
{
    char **servers = (char**)calloc(MAX_SERVERS_ALLOWED, sizeof(char *));
    verify_servers_alloc(servers);
    return servers;
}

void run()
{
    int counter = 0;

    char **servers = allocate_server_space();
    do 
    {
        add_new_to(servers, counter);  
        counter += 1;
    } while(counter < MAX_SERVERS_ALLOWED);

    free_mem(servers);
}


int main(void)
{
    run();
    return 0;
}


