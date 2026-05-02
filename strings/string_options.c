#include <stdio.h>
#include <stdlib.h>

int split_string(char *raw_input, char delimiter, char **left_part, char **right_part)
{
    int counter = 0;
    int left_count = 0;
    int right_count;
    int found = 0;

    while(raw_input[counter] != '\0')
    {
        if(raw_input[counter] == delimiter)
        {
            left_count = counter;
            found++;
        }

        counter++;
    }

    if(found == 0) return 0;

    // need to remove 1 char for the extra counter increment
    right_count = (counter - 1) - left_count;

    // allocate an extra char for the null terminator
    char *left = (char *)malloc(left_count + 1);
    if(left == NULL) exit(1);
    char *right = (char *)malloc(right_count + 1);
    if(right == NULL) exit(1);

    // copy the values to the left pointer
    for(int i = 0; i < left_count; i++)
        left[i] = raw_input[i];

    // copy the values to the right pointer
    for(int i = 0; i < right_count; i++)
        right[i] = raw_input[left_count + 1 + i];
    
    // replace the last char with the null terminator
    // since I already assigned enough space I do not need
    // to perform the -1
    left[left_count] = '\0';
    right[right_count] = '\0';

    *left_part = left;
    *right_part = right;

    return found;
}

int main() {
    char *network_data = "DECK:BATTLEMAGE";
    char *command = NULL;
    char *value = NULL;

    // Notice we pass the ADDRESS of our pointers (&command) -> this becomes char **
    if (split_string(network_data, ':', &command, &value)) {
        printf("Command: %s\n", command); // Should print "Command: DECK"
        printf("Value: %s\n", value);     // Should print "Value: BATTLEMAGE"
        free(command);
        free(value);
    }

    return 0;
}
