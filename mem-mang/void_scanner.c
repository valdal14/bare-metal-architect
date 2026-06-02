#include <stdio.h>

int main(void)
{
    unsigned char stream[16] = {0}; 
    int secret_code = 777;
    
    // The system hides the integer 6 bytes deep into the stream
    *(int *)(stream + 6) = secret_code;
    
    void *vp = (void *)stream;
    int num = *(int *)(vp + 6);
    printf("found num = %d\n", num);
    
    return 0;
}
