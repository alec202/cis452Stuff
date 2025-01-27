// this is our program
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

// Signal handlers
void handle_sigusr1(int sig) {
    printf("Signal received was: SIGUSR1\n");
}

void handle_sigusr2(int sig) {
    printf("Signal received was: SIGUSR2\n");
}

void handle_cntrl_c(int sig) {
    printf("Signal received was Control + C\n");
    printf("Time to exit I'm shutting down...\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    int pid;

    // Fork a child process
    pid = fork();

    if (pid == 0) {
        // In child process
        while (1) {
            // Get a random time 0 to 5 seconds
            int timeToWait = rand() % 6;
            sleep(timeToWait); // Wait for the randomly specified amount of time

            // Randomly choose a signal to send
            int signalToSend = rand() % 2 == 0 ? SIGUSR1 : SIGUSR2;
            // Send the signal to the parent process
            kill(getppid(), signalToSend); 
        }     
    } 
    
    else {
        // In parent process
        // Print out child PID
        printf("Spawned child PID# %d\n", pid);

        // Set up the SIGUSR1 handler
        if (signal(SIGUSR1, handle_sigusr1) == SIG_ERR) {
            perror("Error setting up signal handler for SIGUSR1");
        }

        // Set up the SIGUSR2 handler
        if (signal(SIGUSR2, handle_sigusr2) == SIG_ERR) {
            perror("Error setting up signal handler for SIGUSR2");
        }

        // Set up the SIGINT (Control+C) handler
        if (signal(SIGINT, handle_cntrl_c) == SIG_ERR) {
            perror("Error setting up signal handler for SIGINT");
        }

        // Parent process waits indefinitely for signals
        while (1) {
            pause(); // Wait for a signal to arrive
        }
    }

    return 0;
}
