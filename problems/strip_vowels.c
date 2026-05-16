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

void reverse_string(char *str)
{
    if(str == NULL || *str == '\0') return;

    int str_len = 0;
    int count = 0;

    while(str[str_len] != '\0')
    {
        str_len++;
    }

    for(int i = str_len - 1; i > 0; i--)
    {
        if(count == (str_len / 2)) break;
        char temp = *(str + count);
        *(str + count) = *(str + i);
        *(str + i) = temp;
        count++;
    }
}

void reverse_string_p(char *str)
{
    if(str == NULL || *str == '\0') return;

    char *left = str;
    char *right = str;

    while(*right != '\0')
    {
        right++;
    }
    // right is now pointing AT the \0.
    // take one step back to point to the last actual letter .
    right--;

    // As long as the left memory address is "before" the right memory address
    while(left < right)
    {
        char temp = *left;
        *left = *right;
        *right = temp;

        left++;
        right--;
    }
}

int main(void)
{
    char str[7] = "Valerio";
    reverse_string(str);
    printf("result = %s\n", str);
    reverse_string_p(str);
    printf("result = %s\n", str);
    strip_vowels(str);
    printf("result = %s\n", str);
    return 0;
}
