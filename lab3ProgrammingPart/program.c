// this is our program
#include <stdio.h>
#include <unistd.h>
#include <signal.h> 
#include <stdlib.h>

int main(int argc, char *argv[]){

    int pid;
    pid = fork();

    if (pid == 0){
        // in child process.
        // want to continue randomly choosing a signal so will just do an infinite loop
        while (1){
            // get a random time 0 to 5
            int timeToWait = rand() % 6;

        }
    } else {
        // in parent process.
        // print out child pid
        printf("Spawned child PID# %d\n", pid);
        // ensure SIGUSR1 handler is setup successfully.
        if(signal(SIGUSR1, handle_sigusr1) == SIG_ERR){
            perror("Error setting up signal handler for SIGUSR1");
        }
        // ensure SIGUSR2 handler is setup successfully.
                if(signal(SIGUSR2, handle_sigusr2) == SIG_ERR){
            perror("Error setting up signal handler for SIGUSR2");
        }


    }

    return 0;
}

void handle_sigusr1(int sig){
    printf("Signal recieved was: SIGUSR1\n");
}

void handle_sigusr2(int sig){
    printf("Signal recieved was: SIGUSR2\n");
}

void handle_cntrl_c(int sig){
    printf("Signal received was control + c");
    printf("time to exit after doing cleanup work");
    exit(0);
}