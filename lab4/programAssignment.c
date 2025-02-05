#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// shared storage for keeping track of the number of files accessed like the lab asks.
int fileRequestsReceived = 0;
char* filesEntered[100];
pthread_t* activeThreads[100];
int numOfThreads = 0;
int numFilesEntered = 0;
void* executionThread(void* fileNamePassedToFunction);

void output(){
    printf("Total file requests received was %d.\n", fileRequestsReceived);
    for (int i=0; i < 100; i++){
        free(filesEntered[i]);
    }
}

// setup control c handler.
void handle_cntrl_c(int sig) {
    printf("\nSignal received was Control + C\n");
    output();
     void* result1;
    int joinStatus;
    joinStatus = pthread_join(thread1ID, &result1);
    if (joinStatus != 0) {
        fprintf (stderr, "Join error %d: %s\n", joinStatus, strerror(joinStatus));
        exit(1);
        }
}

int main(){
    // Initialize all elements to 0
    memset(activeThreads, 0, sizeof(activeThreads));

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
        // malloc-ing space on heap for our active threads array.
        pthread_t *threadID = malloc(sizeof(pthread_t)); // Allocate memory for thread ID
        // make sure malloc was succesful.
        if (threadID == NULL) {
            perror("Failed to allocate memory for thread ID");
            return 1;
        }

        fileRequestsReceived++;
        // now that we know the name of the file we want to access let's make the child thread.
        char* fileNameCopy = strdup(fileToAccess); // Duplicate file name
        // put this in our shared memory space
        filesEntered[numFilesEntered] = fileNameCopy;
        // put the threadID in our active threads array.
        activeThreads[numOfThreads] = threadID;
        // increment the number of threads
        numFilesEntered++;
        numOfThreads++;
        threadCreationSuccess = pthread_create(threadID, NULL, executionThread, fileNameCopy);        
        // make sure the thread was created successfully.
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
    // get the ID of this thread.
    pthread_t thisThreadsID = pthread_self();
    // looop through active threads list so we can change it to say this thread is inactive.
    for (int i = 0; i < 100; i++){
        // check if the thread ID is equal at this index in the array to this threads ID
        if (pthread_equal(thisThreadsID, activeThreads[i])){
            // if the thread ID of this thread is equal to the thread ID at this index, change the thread ID at this index to 0
            activeThreads[i] = 0;
        }
    }
    // decrement the number of active threads since we are finished with this thread.
    numOfThreads--;

    return NULL;
}