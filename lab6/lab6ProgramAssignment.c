#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>

#define SIZE 16

typedef struct {
    int value;
    struct process *list;
} semaphore;

void sem_wait(int semid) {
    struct sembuf sb = {0, -1, 0}; 
    semop(semid, &sb, 1);
}

void sem_signal(int semid) {
    struct sembuf sb = {0, 1, 0}; 
    semop(semid, &sb, 1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <loop count>\n", argv[0]);
        exit(1);
    }

    int status;
    long int i, loop, temp, *sharedMemoryPointer;
    int sharedMemoryID, semid;
    pid_t pid;

    loop = atoi(argv[1]);

    // Create shared memory
    sharedMemoryID = shmget(IPC_PRIVATE, SIZE, IPC_CREAT | S_IRUSR | S_IWUSR);
    if (sharedMemoryID < 0) {
        perror("Unable to obtain shared memory");
        exit(1);
    }

    sharedMemoryPointer = shmat(sharedMemoryID, 0, 0);
    if (sharedMemoryPointer == (void *)-1) {
        perror("Unable to attach");
        exit(1);
    }

    sharedMemoryPointer[0] = 0;
    sharedMemoryPointer[1] = 1;

    pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) { // Child process
        for (i = 0; i < loop; i++) {
            sem_wait(semid);
            temp = sharedMemoryPointer[0];
            sharedMemoryPointer[0] = sharedMemoryPointer[1];
            sharedMemoryPointer[1] = temp;
            sem_signal(semid);
        }
        shmdt(sharedMemoryPointer);
        exit(0);
    } else { // Parent process
        for (i = 0; i < loop; i++) {
            
            temp = sharedMemoryPointer[1];
            sharedMemoryPointer[1] = sharedMemoryPointer[0];
            sharedMemoryPointer[0] = temp;
            
        }
        wait(&status);
        printf("Values: %li\t%li\n", sharedMemoryPointer[0], sharedMemoryPointer[1]);

        // Cleanup
        shmdt(sharedMemoryPointer);
        shmctl(sharedMemoryID, IPC_RMID, 0);
        
    }

    return 0;
}