#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

/**
 * @brief Count the number of vowels inside 
 * a string of chars
 * @param const char pointer
 * @return int The number of vowels found
 */
int count_vowels(const char *str)
{
    int counter = 0;
    int vowels_counter = 0;

    while(str[counter] != '\0')
    {
        if(str[counter] % 2 != 0)
        {
            vowels_counter += 1;
        }
        counter += 1;
    }

    return vowels_counter;
}

/**
 * @brief Helper function used to format the message
 * to display based on the given string 
 * @param int count The number of vowels returned by
 * the count_vowels function
 * @param chat word pointer The string for which we
 * counted the vowels
 * @return void
 */
void print_vowels_count(int count, char *word)
{
    switch (count)
    {
        case 0:
            printf("%s does not contain any vowels\n", word);
            break;
        case 1: 
            printf("%s contains only %d vowel\n", word, count);
            break;
        default:
            printf("%s contains %d vowels\n", word, count);
            break;
    }
}

/**
 * @brief Mutates a given string IN PLACE by replacing all chars with '*'
 * @param word A pointer to a MUTABLE string (Heap or Stack Array)
 * @return void
 */
void mask_word(char *word)
{
    int i = 0;
    
    // Just walk the string and overwrite the letters! No malloc needed!
    while(word[i] != '\0')
    {
        word[i] = '*';
        i++;
    }
}

/**
 * @brief Print the value stored inside the dic
 * @param char word[][15] matrix
 * @param int rows
 * @return void
 */
void print_dictionary(char words[][15], int rows)
{
    for(int i = 0; i < rows; i++)
    {
        printf("%s\n", words[i]);
    }
}

int main(void)
{
    char words[3][15] = {"Elisa", "Maria", "Grace"};
    char passkey[] = "Grace";

    int vowels_count = count_vowels("Valerio");
    print_vowels_count(vowels_count, "Valerio");
    
    mask_word(passkey);
    printf("Mask result = %s\n", passkey);

    print_dictionary(words, 3);
    
    return 0;
}
