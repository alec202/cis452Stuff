#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_NODES 10
#define MSG_SIZE 256

typedef struct {
    int sender;
    int receiver;
    char content[MSG_SIZE];
} message_t;

void node_process(int id, int k, int read_pipe[2], int write_pipe[2]) {
    close(read_pipe[1]);  // Close write-end of read_pipe
    close(write_pipe[0]); // Close read-end of write_pipe

    message_t msg;

    while (1) {
        // Read message from the previous node
        close(read_pipe[1]);
        close(write_pipe[0]);
        read(read_pipe[0], &msg, sizeof(msg));
        printf("Node %d received the apple.\n", id);
        if (msg.receiver == id){
            printf("The message was intended for this node (%d)\n", id);
            // copy the message into variable message
            char message[MSG_SIZE];
            strncpy(message, msg.content, MSG_SIZE);
            printf("The message was: %s", message);
            // set the message content to 'empty'
            char msgReceivedStatus[MSG_SIZE] = "empty\0";
            strncpy(msg.content, msgReceivedStatus, MSG_SIZE - 1);
            msg.content[MSG_SIZE - 1] = '\0';
            printf("The contents of the message have been replaced with: %s\n", msg.content);
        } else {
            printf("The message was not intended for this node (%d). Will be sending the apple to the next node.\n", id);
        }
        
        msg.sender = id;
        // Send the message to the next node
        write(write_pipe[1], &msg, sizeof(msg));
    }
}

void handleCntrlC(int sig) {
    printf("\nSignal received was Control + C\n");
    printf("Time to exit I'm shutting down...\n");
    exit(0);
}


int main() {
    int k;
    printf("Enter number of nodes: ");
    scanf("%d", &k);
    if (k < 2 || k > MAX_NODES) {
        printf("Invalid number of nodes (2-%d allowed).\n", MAX_NODES);
        return EXIT_FAILURE;
    }
    // there should be k + 1 nodes in reality since node 0 (the parent) doesn't count.
    k++;
    // create k pipes with size 2 to specify read vs. write
    int pipes[k - 1][2]; // Pipes for communication

    // Create pipes at each index of the 2D pipe array
    for (int i = 0; i < k; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return EXIT_FAILURE;
        }
    }

    // Spawn processes
    for (int i = 1; i < k; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            node_process(i, k, pipes[i], pipes[(i + 1) % k]);
            exit(0);
        }
    }

    // we will want to be able to enter a message more than once.
    while (1){
        close(pipes[0][1]);  // Close write-end of read pipe, pipe 0
        close(pipes[1][0]); // Close read-end of write pipe, pipe 1 since we will be writing to that pipe.
        // get the input to send in the apple.
        int nodeToSendMessageTo;
        puts("Enter the node number you want to send a message to:");
        scanf(" %d", &nodeToSendMessageTo); 
        getchar();
        if (nodeToSendMessageTo > k - 1 || nodeToSendMessageTo < 0){
            printf("Node entered must be within range 0 - %d. Try again!\n", (k - 1));
            continue;
        }
        char messageToSend[MSG_SIZE];
        printf("Enter the message you want to send. Maximum message size is: %d\n", MSG_SIZE);
        // we have to use fgets since we want to be able to take multi word messages
        fgets(messageToSend, MSG_SIZE, stdin);
        // got the input for the apple, now package apple and write to the first pipe.
        // initialize the apple. We can't just use the direct message to send because C doesn't allow for direct
        // assignment. We have to copy the data from the source array to the destination array.
        message_t init_msg = {0, nodeToSendMessageTo, ""};
        // copy the messageToSend into msg.content
        strncpy(init_msg.content, messageToSend, MSG_SIZE - 1);
        // ensure the last character is the null terminator.
        init_msg.content[MSG_SIZE - 1] = '\0';
        if (init_msg.receiver == 0){
            // if the destination node for the message is the parent, then we will handle this scenario in a more unique way.
            printf("The message was intended for this node (0)\n");
            // copy the message into variable message
            char message[MSG_SIZE];
            strncpy(message, init_msg.content, MSG_SIZE);
            printf("The message was: %s", message);
            // set the message content to 'empty'
            char msgReceivedStatus[MSG_SIZE] = "empty\0";
            strncpy(init_msg.content, msgReceivedStatus, MSG_SIZE - 1);
            init_msg.content[MSG_SIZE - 1] = '\0';
            printf("The contents of the message have been replaced with: %s\n", init_msg.content);
            puts("ALL PROCESSES DONE");
            if (strcmp(init_msg.content, "empty\0") == 0){
                puts("Message has reached the beginning node");
                puts("Send another message!");
            }
            // since the message has been sent a received since the destination node was the parent node, we should re-prompt the user
            // while skipping the code below since we don't need to go through a whole loop since it was
            // to the parent node. So we will skip this iteration's code below with the continue statement.
            continue;
        } else {
            // node 0 has already viewed the destination node and determined it's not for them so now we still
            // have to print out the debugging info and then send it to the next node, node 1.
            puts("Node 0 received the apple.");
            puts("The message was not intended for this node (0). Will be sending the apple to the next node.");

            // Write to the first node
            write(pipes[1][1], &init_msg, sizeof(init_msg));
            // read from buffer        
            read(pipes[0][0], &init_msg, sizeof(init_msg));
            puts("ALL PROCESSES DONE");
            
            // Set up the SIGINT (Control+C) handler
            if (signal(SIGINT, handleCntrlC) == SIG_ERR) {
                perror("Error setting up signal handler for SIGINT");
            }
            if (strcmp(init_msg.content, "empty\0") == 0){
                puts("Message has reached the beginning node");
                puts("Send another message!");
            }
        }
    }

    return 0;
}