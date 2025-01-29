#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv){
    int kNodes;
    char userInput[500];
    puts("Enter how many nodes do you want there to be?");
    int kNodeInputSuccess = scanf("%d", &kNodes);
    if (kNodeInputSuccess != 1){
        perror("Scanf failed to get number of nodes user wants");
    }
    // if we create an array of all the process Ids we will have. we can reuse one communication pipe as we
    // loop over this array.
    pid_t processIds[kNodeInputSuccess];



}