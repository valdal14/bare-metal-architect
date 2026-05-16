#include <stdio.h>


void trim_left(char *str)
{
    if(str == NULL || *str == '\0') return;
    if(*(str + 1) != ' ') return;

    char *read = str;
    char *write = str;

    while(*read == ' ') read++;

    while(*read != '\0')
    {
        *write = *read;
        write++;
        read++;
    }

    *write = '\0';
}

void trim_right(char *str)
{
    if(str == NULL || *str == '\0') return;
    
    char *read = str;
    
    while(*read != '\0') read++;

    if(*(read - 1) != ' ') return; 

    while(read > str && *(read - 1) == ' ') read --;

    *read = '\0';
}

void trim(char *str)
{
    if(str == NULL) return;
    trim_left(str);
    trim_right(str);
}

int main(void)
{
    char str1[] = "         Hello Val !   ";
    char str2[] = "         Hello Val !   ";
    char str3[] = "         Hello Val !";
    char str4[] = "Hello Val !   ";
    
    trim_left(str1);
    printf("|%s|\n", str1);
    trim_right(str1);
    printf("|%s|\n", str1);

    trim(str2);
    printf("|%s|\n", str2);

    trim(str3);
    printf("|%s|\n", str3);
    
    trim(str4);
    printf("|%s|\n", str4);

}
