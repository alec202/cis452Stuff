#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


int main(int arc, char *argv[]){
    int fd[2];
    int pipeCreationResult = pipe(fd);
    if (pipeCreationResult < 0){
        perror("Failed to create pipe\n");
        exit(1);
    }
    char myStringInput[500];
    char userInput[500];
    puts("type a message to send to the parent process.");
    int pid = fork();
    
    if (pid == 0){
        // this is the child process.
        close(fd[0]);
        char* fgetsSuccess = fgets(userInput, 500, stdin);
        if (fgetsSuccess == NULL){
            perror("Fgets failed to get input successfully");
        }
        write(fd[1], userInput, sizeof(char) * strlen(userInput));
        printf("Child process wrote user input: %s", userInput);

    } else {
        // this is the parent process.
        close(fd[1]);
        read(fd[0], myStringInput, 500);
        printf("Parent process read: %s", myStringInput);
    }
    return 0;
}
