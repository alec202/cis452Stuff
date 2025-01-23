#include <stdio.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/resource.h> 


int main(int argc, char *argv[]) {
    char userInput[40]; //stores the user input
    char *args[10];     //arry to store the user command and arguements
    pid_t pid;          // process ID of the child    
    struct rusage usage = {0}; // To capture resource usage statistics for the child

    puts("This is a simple command interpreter. Enter 'quit' to exit. Otherwise, enter a command with optional arguments (e.g., 'ls -la').");

    while (1) {
        // Display prompt
        printf("Enter your input: ");

        // Read user input
        //https://www.geeksforgeeks.org/fgets-function-in-c/
        if (fgets(userInput, sizeof(userInput), stdin) == NULL) {
            perror("fgets failed");
            continue;
        }

        //Remove trailing newline character, \n , from fgets
        //We do this since exec() is a system call so that the child witll execute different code from its parent
        for (int i = 0; userInput[i] != '\0'; i++) 
        {
    		if (userInput[i] == '\n') 
    		{
        		userInput[i] = '\0';
        		break; // Exit loop once newline is replaced
    		}
	}

        // Check for "quit" command
        if (strcmp(userInput, "quit") == 0) {
            printf("Terminating program.\n");
            break;
        }

        // take user input and create tokens from it 
        //https://stackoverflow.com/questions/28143113/using-strtok-function-to-tokenize-a-sentence
        char *token = strtok(userInput, " "); //strtok() will split the userInput based on white space " ".
        int n = 0;                            //to index through the arguements
        while (token != NULL ) 
        {
            args[n++] = token;           //store the token in the arg arrary
            token = strtok(NULL, " ");   //get next token
            
            //DEBUGGING
            //printf("print my command %d entered %s\n" , n, token);
        }
        
        // Null-terminate the argument list for execvp()
        args[n] = NULL; 
        //https://stackoverflow.com/questions/28143113/using-strtok-function-to-tokenize-a-sentence

        // Fork a child process
        pid = fork();
        // wait will store the exit status of the child process here
        int status; 
        
        if (pid < 0) {
            perror("fork FAILED.");
            exit(1);
        }

        if (pid == 0) 
        {
            // Child process: Execute the command with a vector of pointers and allows a user to enter a pathname
            if (execvp(args[0], args) < 0) {
                perror("execvp failed");
                exit(1);
            }
        } 
        
        else
        {
            //Parent process: resource usage
            if (wait(&status) < 0)
            {
            	perror("waiting on child");
            }
            else
            {
            	// Calculate user CPU time used by the child process.
        	long seconds = usage.ru_utime.tv_sec;
        	long microseconds = usage.ru_utime.tv_usec;
        	double user_cpu_time = (double)seconds + (double)microseconds / 1000000.0;

        	// Get number of involuntary context switches.
        	long involuntary_context_switches = usage.ru_nivcsw;

        	printf("Child process (PID: %d) used %.6f seconds of user CPU time.\n", pid, user_cpu_time);
        	printf("Child process (PID: %d) experienced %ld involuntary context switches.\n", pid, involuntary_context_switches);
            }
  
       }
        
    }//close while loop

    return 0;
}
