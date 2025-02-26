
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 4096
#define MAX_STRING 256
#define NUM_READERS 2

// Structure must match the one in writer.c
struct shared_data {
    char message[MAX_STRING];    // The shared message
    int reader_arr[NUM_READERS];
    int readers_done;            // Counter for readers who have finished
    int writer_done;            // Flag to indicate writer has finished
};

// Global variables for cleanup
int shmId;
struct shared_data* shm_ptr;

// Signal handler for cleanup
void cleanup(int sig) {
    shmdt(shm_ptr);             // Detach from shared memory
    exit(0);
}

int main(int argc, char *argv[]) {
    // Generate the same key as writer
    key_t key = ftok("writer.c", 22);
    
    // Get existing shared memory segment
    if ((shmId = shmget(key, SHM_SIZE, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    
    // Attach to shared memory segment
    if ((shm_ptr = shmat(shmId, NULL, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }
    
    // Set up signal handler
    signal(SIGINT, cleanup);
    
    int reader_id = atoi(argv[1]);
    
    // Main reading loop
    while (1) {
        // Check if writer has finished, its sets to 1 
        if (shm_ptr->writer_done) {
            cleanup(0);
        }
        
         if (shm_ptr->readers_done < 2) {
            printf("Read: %s\n", shm_ptr->message);
            shm_ptr->reader_arr[reader_id] = 1;
            shm_ptr->readers_done++;  // Increment readers counter
        }
    }
    
    return 0;
}