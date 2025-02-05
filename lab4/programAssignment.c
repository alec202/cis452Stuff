#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// shared storage for keeping track of the number of files accessed like the lab asks.
int fileRequestsReceived = 0;
pthread_t thread1ID;
char* filesEntered[100];
int numFilesEntered = 0;
void* executionThread(void* fileNamePassedToFunction);

void output(){
    printf("Total file requests received was %d.\n", fileRequestsReceived);
    void* result1;
    int joinStatus;
    joinStatus = pthread_join(thread1ID, &result1);
    if (joinStatus != 0) {
        fprintf (stderr, "Join error %d: %s\n", joinStatus, strerror(joinStatus));
        exit(1);
        }
    exit(0);
}

// setup control c handler.
void handle_cntrl_c(int sig) {
    printf("\nSignal received was Control + C\n");
    output();
    for (int i=0; i < 100; i++){
        free(filesEntered[i]);
    }
}

int main(){
    int threadCreationSuccess;
    char fileToAccess[20];
    puts("This is a program to simulate file accesses.");
    // Set up the SIGINT (Control+C) handler
    if (signal(SIGINT, handle_cntrl_c) == SIG_ERR) {
        perror("Error setting up signal handler for SIGINT");
    }
    // loop to simulate file access.
    while (strcmp(fileToAccess, "quit") != 0){
        // get name of file
        puts("Enter the name of the file you want to access or enter 'quit' to stop.");
        fgets(fileToAccess, 20, stdin);
        //Remove trailing newline character, \n , from fgets
        for (int i = 0; fileToAccess[i] != '\0'; i++) 
        {
    		if (fileToAccess[i] == '\n') 
    		{
        		fileToAccess[i] = '\0';
        		break; // Exit loop once newline is replaced
    		}
	    }
        if (strcmp(fileToAccess, "quit") == 0){
            break;
        }

        fileRequestsReceived++;
        // now that we know the name of the file we want to access let's make the child thread.
        char* fileNameCopy = strdup(fileToAccess); // Duplicate file name
        filesEntered[numFilesEntered] = fileNameCopy;
        numFilesEntered++;
        threadCreationSuccess = pthread_create(&thread1ID, NULL, executionThread, fileNameCopy);        // make sure the thread was created successfully.
        if (threadCreationSuccess != 0){
            fprintf (stderr, "thread create error %d: %s\n", threadCreationSuccess, strerror(threadCreationSuccess));
        }
    }

    output();
    puts("End of program, goodbye.");
    return 0;
}

void* executionThread(void* fileNamePassedToFunction){
    // use this variable below instead of fileNamePassedToFunction as it's been casted to a string whereas fileNamePassedToFunction is of type void *.
    char* fileName = (char *)fileNamePassedToFunction;
    // get a random number in range 1 - 10: https://www.geeksforgeeks.org/generating-random-number-range-c/
    int randNum = rand() % (10) + 1;
    // 20% of the time, if the file is in the hdd v
    if (randNum < 3){
        // sleep for a random amount of time from 7 - 10 seconds
        int randSleepAmount = rand() % (4) + 7;
        sleep(randSleepAmount);
    // the file was found in the cache so we sleep for only 1 second.
    } else {
        sleep(1);
    }
    printf("File %s has been accessed successfully.\n", fileName);
    free(fileName); // Free the allocated memory

    return NULL;
}