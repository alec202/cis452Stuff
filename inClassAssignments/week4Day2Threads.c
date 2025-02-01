#include <pthread.h>
#include <stdio.h>

void* happy(void* arg) {
    int* N = (int *)arg;
    for (int k = 0; k < *N; k++)
        printf("Happy %d\n", k);
    char* word = "apples";
    return (void *) word;
}

int main(){
    pthread_t happy_id;
    pthread_attr_t happy_attr;

    pthread_attr_init (&happy_attr);
    
    int threadArgument;
    threadArgument = 10;
    pthread_create(&happy_id, &happy_attr, happy, &threadArgument);
    for (int k = 0; k < 20; k++)
        printf("Before %d\n", k);
    
    int joinReturnValue;
    pthread_join(happy_id, (void **) &joinReturnValue);
    printf("Thread status is %x\n", joinReturnValue);
    
    printf("End of program\n");

    return 0;
}