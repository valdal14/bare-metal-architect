#include <stdio.h>

void strip_vowels(char *str)
{
    int count = 0;
    int s_count = 0;

    while(*(str + count) != '\0')
    {
        if((*(str + count) != 'a' && *(str + count) != 'A') &&
           (*(str + count) != 'e' && *(str + count) != 'E') &&
           (*(str + count) != 'i' && *(str + count) != 'I') &&
           (*(str + count) != 'o' && *(str + count) != 'O') &&
           (*(str + count) != 'u' && *(str + count) != 'U'))
        {
            *(str + s_count) = *(str + count);
            s_count++;
        }
        
        count++;
    } 
    
    *(str + s_count) = '\0';
}

int main(void)
{
    char str[7] = "Valerio";
    strip_vowels(str);
    printf("result = %s\n", str);
    return 0;
}
