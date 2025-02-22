#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>

int main() {
	//creates a semaphore with the name semtest, 
	//set permissions to read/write for owner, read-only for others
	//the initial value is 0
    sem_t *sem = sem_open("testsem", O_CREAT, 0644, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        return 1;
    }
    //set int count to track value of semaphore
    int count = 0;
    //sem_post increments the semaphore by 1
    //while loop continue until sem_post fails
    while (sem_post(sem) == 0) {
        count++;
    }
    printf("Maximum semaphore value: %d\n", count);
    //close the semaphore
    sem_close(sem);
    //deletes the semaphore
    sem_unlink("/testsem");
    return 0;
}
