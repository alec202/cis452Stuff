
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SIZE 4096
#define STRING_LEN 255
#define NUM_READERS 2

struct shared_data {
    char message[STRING_LEN]; //the shared message between writers and readers
    int reader_arr[NUM_READERS];  //tracks which readers have got the message
    int readers_count;        //counter for when readers are done
    int writer_flag;          //set when to write
};

int shmId;                    //shared memory segment ID
struct shared_data* shm_ptr;  //pointer to shared memory

//signal handler to release the resources 
void release(int signum) {
    printf("\nWriter exiting. RELEASE shared memory.\n");
    shmdt(shm_ptr);                 //this detaches from shared memory
    shmctl(shmId, IPC_RMID, NULL);  //removes shared memory segment
    exit(0);
}

int main() {
    //access the writers resources using the passkey 22 
    key_t key = ftok("writer.c", 22);
    
    //from lab, creates shared memory segment
    if ((shmId = shmget(key, SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    
    //from lab, attaches to shared memory
    if ((shm_ptr = shmat(shmId, NULL, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }
    
    //ctrl + c interrupt 
    signal(SIGINT, release);
    
    // Initialize shared memory state
    shm_ptr->readers_count = 2;  // Initialize for two readers
    shm_ptr->writer_flag = 0;    //writer not done
    for (int i = 0; i < NUM_READERS; i++) {
        shm_ptr->reader_arr[i] = 0;
    }
    
    char input[STRING_LEN];
    while (1) {
        //if both writers are available then write
        if (shm_ptr->readers_count == 2) {
            printf("Enter message (or 'quit' to exit): ");
            fgets(input, STRING_LEN, stdin);
            input[strcspn(input, "\n")] = 0;
            
            //if 'quit' is entered
            if (strcmp(input, "quit") == 0) {
                shm_ptr->writer_flag = 1;
                release(0);
            }
            
            //write message to shared data via the pointer
            strcpy(shm_ptr->message, input);
            //readers are 0 so they can read now
            shm_ptr->readers_count = 0;
            
            for (int i = 0; i < NUM_READERS; i++) {
                shm_ptr->reader_arr[i] = 0;
            }
        }
    }
    
    return 0;
}