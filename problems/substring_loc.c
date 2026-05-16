#include <stdio.h>

/**
 * @brief Finds the first occurrence of a substring within a string.
 * * @param haystack The main string to search within.
 * @param needle The substring to search for.
 * @return char* Pointer to the beginning of the substring within the haystack, 
 * or NULL if the substring is not found.
 */
char *find_substring(const char *haystack, const char *needle)
{
    if (needle == NULL || *needle == '\0') return (char *)haystack;
    if (haystack == NULL || *haystack == '\0') return NULL; // Not found!

    const char *h = haystack; 

    while (*h != '\0')
    {
        const char *h_scout = h;
        const char *n_scout = needle;

        while (*h_scout != '\0' && *n_scout != '\0' && *h_scout == *n_scout)
        {
            h_scout++;
            n_scout++;
        }

        if (*n_scout == '\0')
        {
            return (char *)h; 
        }

        h++;
    }

    return NULL; // Never found it
}

int main(void)
{
    char h[] = "Systems Engineering";
    char n[] = "Engine";
    
    char *match = find_substring(h, n);
    
    if (match != NULL)
    {
        printf("Found substring starting at: %s\n", match); 
    }
    else
    {
        printf("Substring not found.\n");
    }
    
    return 0;
}
