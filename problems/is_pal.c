#include <stdio.h>
#define UPPER_OFFSET 32

int is_alphanumeric(char c) 
{
    if ((c >= 'A' && c <= 'Z') || 
        (c >= 'a' && c <= 'z') || 
        (c >= '0' && c <= '9')) 
    {
        return 1;
    }
    return 0;
}

char to_lowercase(char c) 
{
    if (c >= 'A' && c <= 'Z') 
    {
        return c + UPPER_OFFSET; 
    }
    return c;
}

int is_palindrome(char *s) 
{
    int size = 0;
    while (*(s + size) != '\0') size++;

    int left = 0;
    int right = size - 1;

    while (left < right) 
    {
        while (left < right && !is_alphanumeric(*(s + left))) 
        {
            left++;
        }

        while (left < right && !is_alphanumeric(*(s + right))) 
        {
            right--;
        }

        if (to_lowercase(*(s + left)) != to_lowercase(*(s + right))) return 0; 

        left++;
        right--;
    }

    return 1;
}

int main(void) 
{
    char str[] = "A dog! A panic in a pagoda!";
    is_palindrome(str) == 1 ? printf("✅ Palindrome\n") : printf("❌ NOT Palindrome\n"); 
    
    return 0;
}
