#include <stdio.h>
#include <pthread.h>

#define MAGICK_NUM 14 

void *routine(void *arg)
{
    int *val = (int *)arg;
    if(val == NULL) return NULL;
    *val += MAGICK_NUM;
    return (void *)val;
}

int main(void)
{
    int num = 0;
    int *result = NULL;
    pthread_t t;
    
    int t1 =  pthread_create(&t, NULL, routine, (void *)&num);
    if(t1 != 0) return -1;

    int j1 = pthread_join(t, (void **)&result); 
    if(j1 != 0) return -1;
    
    printf("Num + MN = %d\n", *result);

    return 0;
}
