#include <stdio.h>

#define UPPER_TO_LOWER 32

/**
 * @brief Helper function to calculate the size of a char *
 * after purification
 * @param char s pointer
 * @return int The size of the purified string
 */
int calc_pur_str_length(char *s)
{
    int counter = 0;
    int str_counter = 0;

    while(s[counter] != '\0')
    {
        if((s[counter] >= 48 && s[counter] <= 57) || 
           (s[counter] >= 65 && s[counter] <= 90) || 
           (s[counter] >= 97 && s[counter] <= 122)) str_counter += 1;
        
        counter += 1;
    }
    
    return str_counter;
}

/*
 * Numbers: '0' (48) through '9' (57)
 * Uppercase: 'A' (65) through 'Z' (90)
 * Lowercase: 'a' (97) through 'z' (122)
 */
int is_palindrome(char *s)
{
    int counter = 0;
    int str_length = calc_pur_str_length(s);
    char purified_str[str_length]; 
    int purified_str_size = 0;

    while(s[counter] != '\0')
    {
        if((s[counter] >= 48 && s[counter] <= 57) || (s[counter] >= 97 && s[counter] <= 122))
        {
            purified_str[purified_str_size] = s[counter];
            purified_str_size += 1;
        }

        if(s[counter] >= 65 && s[counter] <= 90)
        {
            purified_str[purified_str_size] = s[counter] + UPPER_TO_LOWER;
            purified_str_size += 1;
        }
        
        counter += 1;
    }
    
    for(int i = 0; i < str_length / 2; i++)
    {
        if((int)purified_str[i] != (int)purified_str[str_length - 1 - i])
            return -1;
    }

    return 0;
}

/**
 * @brief Prints the message based on the int value
 * @param int value
 * @return void
 */
void print_response(int value)
{
    if(value == 0)
    {
        printf("True the string is a palindrome\n\n");
    }
    else
    {
        printf("False the string is not a palindrome\n\n");
    }
}


int main(void)
{
    print_response(is_palindrome("A man, a plan, a canal: Panama"));
    print_response(is_palindrome("Cocktail"));
    print_response(is_palindrome("RaceCar"));
    print_response(is_palindrome("Madam"));
    print_response(is_palindrome(""));
    print_response(is_palindrome("liilii"));
    print_response(is_palindrome("level"));

    return 0;
}
