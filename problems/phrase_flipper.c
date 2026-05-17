#include <stdio.h>

/**
 * @brief Reverses a specific chunk of memory between two pointers.
 */
void reverse_chunk(char *left, char *right)
{
    while(left < right)
    {
        char temp = *left;
        *left = *right;
        *right = temp;
        left++;
        right--;
    }
}

void reverse_words(char *str)
{
    if(str == NULL || *str == '\0') return;

    // --- STEP 1: Reverse the entire string ---
    char *end = str;
    while(*end != '\0') 
    {
        end++;
    }
    // 'end' is at '\0', so pass 'end - 1' to reverse the actual characters
    reverse_chunk(str, end - 1); 

    // --- STEP 2: Reverse each individual word ---
    char *word_start = str;
    char *word_end = str;

    while(*word_start != '\0')
    {
        // 1. Scout forward to find the end of the current word
        word_end = word_start;
        while(*word_end != ' ' && *word_end != '\0')
        {
            word_end++;
        }

        // 2. Reverse just that chunk! 
        // word_end is pointing at the space (or \0), so the last letter is word_end - 1
        reverse_chunk(word_start, word_end - 1);

        // 3. Move the start pointer to the beginning of the next word
        if(*word_end == '\0')
        {
            break; // We hit the end of the string, we are done!
        }
        else
        {
            word_start = word_end + 1; // Skip over the space
        }
    }
}

int main(void)
{
    char str[] = "Systems Engineering CLang";
    reverse_words(str);
    printf("|%s|\n", str); // Expected: |CLang Engineering Systems|
    return 0;
}