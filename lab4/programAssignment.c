#include <stdio.h>
#include <pthread.h>
#include <string.h>

// shared storage for keeping track of the number of files accessed like the lab asks.
int fileRequestsReceived = 0;
// assume there's some upper bound, we can choose what that will be. In our case we can do 100 just to hope it doesn't wait a super long time.
int main(){
    int threadCreationSuccess;
    pthread_t thread1ID;
    char fileToAccess[20];
    puts("This is a program to simulate file accesses.");
    while (strcmp(fileToAccess, "quit\n") != 0){
        // get name of file
        puts("Enter the name of the file you want to access or enter 'quit' to stop.");
        fgets(fileToAccess, 20, stdin);
        if (strcmp(fileToAccess, "quit\n") == 0){
            break;
        }
        fileRequestsReceived++;
        // now that we know the name of the file we want to access let's make the child thread.
        // threadCreationSuccess = pthread_create(&thread1ID, NULL, executionThread, fileToAccess);
    }

    puts("End of program, goodbye.");
    return 0;
}

void executionThread(void* fileNamePassedToFunction){

}