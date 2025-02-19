#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>

int main()
{
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } arg;
    arg.val = 1;

    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if (semid == -1) {
        perror("semget failed");
        exit(1);
    }

    for (long int i = 4096; i < 2147483647; i++){
        arg.val = i;
        if (semctl(semid, 0, SETVAL, arg) == -1) {
            printf("Maximum counting semaphore value: %ld\n", i);
            perror("semctl SETVAL failed");
            break;
        }

    }

    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl IPC_RMID failed");
    }
    puts("The Maximum value for a semaphore was not reached");
    return 0;
}
