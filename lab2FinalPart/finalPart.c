#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    char userInput[40];
    puts("this is a simple command interperter. Enter 'quit' to exit. Otherwise enter a command and an argument with it, for example ls -la");
    while (1){
        puts("Enter your input");
        // fgets includes a newline character at the end of the user input automatically which is why we check for "quit\n" instead of just
        // "quit"
        if (!strcmp (userInput, "quit\n")){
            printf("terminating program.\n");
            break;
        }
        // fgets includes a newline character at the end of the user input automatically
        fgets(userInput, 40, stdin);
        printf("string: %s", userInput);
        // execvp return -1 if it failed to execute.
        // we're going to want to start a child process to execute the command.
        // recall from class if we use fork(), if fork() = to 0, then it's the child processes. otherwise for the parent process, it will be
        // the PID (Process ID).

        // Check if we have the child process.
        if (fork() == 0){
            puts("In child process before execution of the command");

            // use strtok to tokenize the inputted string based on spaces. It will return null when there's no more tokens (words/arguments). 
            char* tokenFromUserInput = strtok(userInput, " ");

            // If we're in the child process, we are going to want to execute the command here.
            // if execvp fails it returns -1. So check if it failed, if it did, then print to error that it failed and exit.
            char command[40];
            // create a copy of the command, since if we don't the next call to strtok will also modify the command variable
            // since it's just a pointer.
            memcpy(command, tokenFromUserInput, strlen(tokenFromUserInput) + 1);
            printf("the tokenizedUserInput currently is: %s\n", tokenFromUserInput);
            tokenFromUserInput = strtok(NULL, " ");
            printf("the tokenizedUserInput after first token is generated is: %s\n", tokenFromUserInput);
            printf("command after first token is generated is: %s\n", command);
            // if (execvp(command, tokenFromUserInput) < 0) {
            //         perror("exec failed");
            //         exit(1);
            // }
            puts("After the exec");
        } 
        else {
            // this is the parent process. So, the C program will come here.
            // puts("This is the parent process printing out.");

        }
    }
    return 0;

    // https://www.digitalocean.com/community/tutorials/execvp-function-c-plus-plus was used to learn about the execvp() command
 }
