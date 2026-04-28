#include <stdio.h>

#define UPPER_TO_LOWER 32

/**
 * @brief Calculates the total raw length of a null-terminated string.
 *
 * @param s Pointer to the string.
 * @return int The number of characters before the null terminator.
 */
int get_length(char *s)
{
    int len = 0;
    while(s[len] != '\0')
    {
        len++;
    }
    return len;
}

/**
 * @brief Checks if a character is alphanumeric using ASCII bounds.
 *
 * @param c The character to evaluate.
 * @return int 1 if the character is a letter or number, 0 otherwise.
 */
int is_alphanumeric_custom(char c)
{
    if((c >= '0' && c <= '9') || 
       (c >= 'A' && c <= 'Z') || 
       (c >= 'a' && c <= 'z'))
    {
        return 1;
    }
    return 0;
}

/**
 * @brief Converts an uppercase letter to lowercase using ASCII offset.
 *
 * @param c The character to convert.
 * @return char The lowercase character, or the original if it wasn't uppercase.
 */
char to_lowercase_custom(char c)
{
    if(c >= 'A' && c <= 'Z')
    {
        return c + UPPER_TO_LOWER;
    }
    return c;
}

/**
 * @brief Evaluates if a string is a palindrome strictly without standard C libraries.
 *
 * This function utilizes an in-place two-pointer approach to traverse the string 
 * from both ends towards the center. It uses custom helper functions to bypass 
 * non-alphanumeric characters and handle case-insensitivity, ensuring O(1) 
 * memory usage and O(n) time complexity.
 *
 * @param s Pointer to the null-terminated string to be evaluated.
 * @return int 0 if the string is a palindrome, -1 if it is not.
 */
int is_palindrome(char *s)
{
    int left = 0;
    int right = get_length(s) - 1;

    // The pointers move towards each other. Because they stop when left >= right, 
    // it naturally implements the "skip half of it" symmetry trick!
    while(left < right)
    {
        // Move left pointer forward if the current char is a space or punctuation
        while(left < right && !is_alphanumeric_custom(s[left])) 
        {
            left++;
        }
        
        // Move right pointer backward if the current char is a space or punctuation
        while(left < right && !is_alphanumeric_custom(s[right])) 
        {
            right--;
        }

        // Compare characters after converting to lowercase
        if(to_lowercase_custom(s[left]) != to_lowercase_custom(s[right])) 
        {
            return -1; // Mismatch found
        }

        // Advance both pointers for the next comparison
        left++;
        right--;
    }

    return 0; // The pointers crossed the middle perfectly
}

/**
 * @brief Prints the message based on the int value.
 * * @param value The result from the is_palindrome evaluation (0 for true, -1 for false).
 * @return void
 */
void print_response(int value)
{
    if(value == 0)
    {
        printf("True the string is a palindrome\n");
    }
    else
    {
        printf("False the string is not a palindrome\n");
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
