#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
    char userInput[40];
    char *args[10];
    pid_t pid;

    puts("This is a simple command interpreter. Enter 'quit' to exit. Otherwise, enter a command with optional arguments (e.g., 'ls -la').");

    while (1) {
        // Display prompt
        printf("Enter your input: ");
        //fflush(stdout);

        // Read user input
        if (fgets(userInput, sizeof(userInput), stdin) == NULL) {
            perror("fgets");
            continue;
        }

        // Remove trailing newline from fgets
        userInput[strcspn(userInput, "\n")] = 0;

        // Check for "quit" command
        if (strcmp(userInput, "quit") == 0) {
            printf("Terminating program.\n");
            break;
        }

        // Tokenize user input
        char *token = strtok(userInput, " ");
        int n = 0;
        while (token != NULL ) 
        {
            args[n++] = token;
            token = strtok(NULL, " ");
            
            printf("print my command %d entered %s\n" , n, token);
        }
        args[n] = NULL; // Null-terminate the argument list
        //https://stackoverflow.com/questions/28143113/using-strtok-function-to-tokenize-a-sentence

        // Fork a child process
        pid = fork();
        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) 
        {
            // Child process: Execute the command
            if (execvp(args[0], args) < 0) {
                perror("execvp failed");
                exit(1);
            }
        } 
            else{
            //do something
            }
        
    }

    return 0;
}

