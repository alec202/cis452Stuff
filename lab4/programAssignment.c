#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// shared storage for keeping track of the number of files accessed like the lab asks.
int fileRequestsReceived = 0;
pthread_t* activeThreads[100];
int numOfThreads = 0;
int numFilesEntered = 0;
void* executionThread(void* fileNamePassedToFunction);

void output(){
    printf("Total file requests received was %d.\n", fileRequestsReceived);
        // we want to join any threads that are still active.
        // Loop over our active thread pointer array
        for (int i = 0; i < 100; i++){
        void* result1;
        int joinStatus;
        // get the thread id pointer for the thread at the current index.
        pthread_t *threadID = activeThreads[i];
        // make sure the thread is active (if it's not equal to NULL).
        if (threadID != NULL){
            // If the thread is active we have to clean it up.
            joinStatus = pthread_join(*threadID, &result1);
            if (joinStatus != 0) {
                fprintf (stderr, "Join error %d: %s\n", joinStatus, strerror(joinStatus));
                exit(1);
            }

        }
    }

}

// setup control c handler.
void handle_cntrl_c(int sig) {
    printf("\nSignal received was Control + C\n");
    output();
}

int main(){
    // Initialize all elements to NULL
    for (int i = 0; i < 100; i++) {
        activeThreads[i] = NULL;
    }
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
        // We have to malloc and use the heap like professor said in class otherwise when passing file input this variable will be changed to the last input in the file.
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
        // put the threadID in our active threads array.
        activeThreads[numOfThreads] = threadID;
        // increment the number of threads
        numFilesEntered++;
        numOfThreads++;
        threadCreationSuccess = pthread_create(threadID, NULL, executionThread, fileNameCopy);        
        // make sure the thread was created successfully.
        if (threadCreationSuccess != 0){
            fprintf (stderr, "thread create error %d: %s\n", threadCreationSuccess, strerror(threadCreationSuccess));
            free(fileNameCopy);
            free(threadID);
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
        // check if the thread ID at this index in the array is equal to this current threads ID.
        // DO NOT CHANGE activeThreads[i] to *activeThreads[i]. IT will make the program unable to terminate.
        if (activeThreads[i] != NULL && pthread_equal(thisThreadsID, activeThreads[i])){
            // if the thread ID of this thread is equal to the thread ID at this index, change the thread ID at this index to NULL
            free(activeThreads[i]);
            activeThreads[i] = NULL;
        }
    }
    // decrement the number of active threads since we are finished with this thread.
    free(fileName);
    numOfThreads--;
    return NULL;
}