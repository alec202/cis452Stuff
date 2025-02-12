#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define SIZE 16

int main(int argc, char *argv[])
{
    int status;
    long int i, loop, temp, *sharedMemoryPointer;
    int sharedMemoryID;
    pid_t pid;
    // user input when running the program.
    loop = atoi(argv[1]);
    sharedMemoryID = shmget(IPC_PRIVATE, SIZE, IPC_CREAT | S_IRUSR | S_IWUSR);
    if (sharedMemoryID < 0)
    {
        perror("Unable to obtain shared memory\n");
        exit(1);
    }

    sharedMemoryPointer = shmat(sharedMemoryID, 0, 0);

    if (sharedMemoryPointer == (void *)-1)
    {
        perror("Unable to attach\n");
        exit(1);
    }

    sharedMemoryPointer[0] = 0;
    sharedMemoryPointer[1] = 1;
    pid = fork();
    if (pid < 0)
    {
        printf("Fork failed\n");
    }

    if (pid == 0)
    { // Child
        for (i = 0; i < loop; i++)
        {
            temp = sharedMemoryPointer[0];
            sharedMemoryPointer[0] = sharedMemoryPointer[1];
            sharedMemoryPointer[1] = temp;
            // swap the contents of sharedMemoryPointer[0] and sharedMemoryPointer[1]
        }
        if (shmdt(sharedMemoryPointer) < 0)
        {
            perror("Unable to detach\n");
            exit(1);
        }
        exit(0);
    }
    else
    // parent process
        for (i = 0; i < loop; i++)
        {
            temp = sharedMemoryPointer[1];
            sharedMemoryPointer[1] = sharedMemoryPointer[0];
            sharedMemoryPointer[0] = temp; 
            // swap the contents of sharedMemoryPointer[1] and sharedMemoryPointer[0]
        }

    wait(&status);
    // sharedMemoryPointer[0] initially was 0
    // sharedMemoryPointer[1] initially was 1
    printf("Values: shared Memory pointer [0] %li\t shared memory pointer [1] %li\n", sharedMemoryPointer[0], sharedMemoryPointer[1]);
    if (shmdt(sharedMemoryPointer) < 0)
    {
        perror("Unable to detach\n");
        exit(1);
    }
    if (shmctl(sharedMemoryID, IPC_RMID, 0) < 0)
    {
        perror("Unable to deallocate\n");
        exit(1);
    }
    return 0;
}
