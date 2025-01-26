// this is our program
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[]){

    int pid;
    pid = fork();

    if (pid == 0){
        // in child process.
    } else {
        // in parent process.
        printf("Spawned child PID# %d\n", pid);
    }

    return 0;
}

void SIGUSR1(){

}
