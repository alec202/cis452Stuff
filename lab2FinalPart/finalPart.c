#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    char userInput[40];
    while (1){
        if (!strcmp (userInput, "quit")){
            printf("terminating program.\n");
            break;
        }
        fgets(userInput, 40, stdin);
        printf("string: %s", userInput);

    }
//     if (argc < 3) {
//         fputs("Usage: must supply a command\n", stderr);
//         exit(1);
//     }
//     puts("Before the exec");
//     if (execvp(argv[1], &argv[1]) < 0) {
//         perror("exec failed");
//         exit(1);
//     }
//     puts("After the exec");
    return 0;
 }
