#include <stdio.h>
#include <string.h>

void send_response(int code)
{
    printf("Delivering the response...\n");

    if(code == 200)
    {
        printf("Success!!!\n");
    }
    else
    {
        printf("Failed -:( \n");
    }
}

void execute(void (*callback)(int), int status)
{
    callback(status);
}

int main(void)
{
    execute(send_response, 201);
    return 0;
}
