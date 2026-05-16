#include <stdio.h>

void compress_spaces(char *str)
{
    if(str == NULL || *str == '\0') return;

    char *read = str;
    char *write = str;

    while(*read != '\0')
    {
        if(*read == ' ')
        {
            *write = *read;
            write++;
            read++;

            while(*read == ' ') read++;
        }
        else
        {
            *write = *read;
            write++;
            read++;
        }
    }
    
    *write = '\0';
}

int main(void)
{
    char str[] = "Data    Engineering   is  fun";
    compress_spaces(str);
    printf("%s\n", str);
    return 0;
}
