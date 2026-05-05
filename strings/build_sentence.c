#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Concatenates an array of words into a single dynamically allocated string,
 *        separated by spaces, and strictly null-terminated.
 * @param words Array of string pointers.
 * @param num_words The number of strings in the array.
 * @return char* Pointer to the newly allocated sentence on the heap.
 */
char* build_sentence(char **words, int num_words)
{
    // start at length num_words - 1 to avoid 
    // adding an extra space. 
    int phrase_lenth = num_words - 1;
    int char_counter = 0;
    // ASCII space value
    int char_space = 32;

    for(int i = 0; i < num_words; i++)
    {
        phrase_lenth += strlen(*(words + i));
    }

    char *sentence = (char *)calloc(1, phrase_lenth + 1);
    if(sentence == NULL) exit(1);
    // add the null terminator 
    sentence[phrase_lenth - 1] = '\0';
    
    for(int i = 0; i < num_words; i++)
    {
        int single_word_size = strlen(*(words + i));
        
        for(int j = 0; j < single_word_size; j++)
        {
            sentence[char_counter] = words[i][j];
            char_counter++;
        }
       
        // add the space
        if(char_counter < phrase_lenth)
        {
            sentence[char_counter] = char_space;
            char_counter++;
        }
    }

    return sentence;
}

int main(void)
{
    // An array of 4 string pointers
    char *dictionary[] = {"Building", "sockets", "in", "C"};
    int count = 4;

    char *result = build_sentence(dictionary, count);

    if (result != NULL) {
        printf("Result: '%s'\n", result);
        free(result);
    }

    return 0;
}
