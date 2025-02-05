#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define FOO 4096
int main()
{
    int shmId;
    char *sharedMemoryPtr;
    struct shmid_ds shm_info;
    
    if ((shmId = shmget(IPC_PRIVATE, FOO, IPC_CREAT | S_IRUSR | S_IWUSR)) < 0)
    {
        perror("Unable to get shared memory\n");
        exit(1);
    }
    if ((sharedMemoryPtr = shmat(shmId, 0, 0)) == (void *)-1)
    {
        perror("Unable to attach\n");
        exit(1);
    }
    printf("Value a: %p\t Value b: %p\n", (void *)sharedMemoryPtr, (void *)sharedMemoryPtr + FOO);
    if (shmctl(shmId, IPC_STAT, &shm_info) == -1) {
        perror("shmctl failed to get info about the shared memory region.");
        return 1;
    }

    printf("Shared memory size: %ld bytes\n", (long)shm_info.shm_segsz);
    
    if (shmdt(sharedMemoryPtr) < 0)
    {
        perror("Unable to detach\n");
        exit(1);
    }
    if (shmctl(shmId, IPC_RMID, 0) < 0)
    {
        perror("Unable to deallocate\n");
        exit(1);
    }
    return 0;
}
