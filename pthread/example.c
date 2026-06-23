#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)

void *calculate_math(void *arg) {
    // cast back to int to get the given int num 
    int *num = (int *)arg; 
    // Allocate space on the shared Heap for the answer
    int *answer = (int *)malloc(sizeof(int));
    *answer = 42 + *num;

    // Return the POINTER to the answer
    return answer;
}

int main(void)
{
    int num = 10;

    pthread_t thread_id;
    // cast num to void pointer to give it to the create function
    pthread_create(&thread_id, NULL, calculate_math, (void *)&num);

    // Create a pointer to hold the incoming data
    int *thread_result = NULL;

    // We pass the ADDRESS of our pointer (&thread_result) to join.
    // The OS forces our pointer to point to the thread's malloc'd answer.
    pthread_join(thread_id, (void **)&thread_result);

    printf("The thread calculated: %d\n", *thread_result);

    // main takes responsibility for freeing the thread's memory!
    free(thread_result);

    return 0;
}
