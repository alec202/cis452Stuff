#include <stdio.h>
#include <sys/stat.h>
#include <sys/shm.h>

int main()
{
    int shmId;
    char *sharedMemoryPtr;
    struct shmid_ds shm_info;

    for (long int i = 4096; i < 2147483646; i++)
    if ((shmId = shmget(IPC_PRIVATE, i, IPC_CREAT | S_IRUSR | S_IWUSR)) < 0)
    {
        printf("Maximum Shared memory size reached. It's : %ld\n", i);
        perror("Unable to get shared memory\n");
        return (1);
    } else {
        // Successfully created the shared memory so this i value isn't the limit, so lets remove it and then let the loop continue
            if (shmctl(shmId, IPC_RMID, 0) < 0)
        {
            perror("Unable to deallocate\n");
            return (1);
         }

    }
    puts("The Maximum size for shared memory possible was not reached");
    return 0;
}