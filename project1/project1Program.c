#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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
        read(read_pipe[0], &msg, sizeof(msg));
        printf("Node %d received message: [%s] from Node %d\n", id, msg.content, msg.sender);

        if (strcmp(msg.content, APPLE) == 0) {
            if (msg.receiver == id) {
                printf("Node %d has received its message!\n", id);
                strcpy(msg.content, "EMPTY");
            }
            msg.sender = id;
            // Send the message to the next node
            write(write_pipe[1], &msg, sizeof(msg));
        }
    }
}

int main() {
    int k;
    printf("Enter number of nodes: ");
    scanf("%d", &k);
    if (k < 2 || k > MAX_NODES) {
        printf("Invalid number of nodes (2-%d allowed).\n", MAX_NODES);
        return EXIT_FAILURE;
    }

    // create k pipes with size 2 to specify read vs. write
    int pipes[k][2]; // Pipes for communication

    // Create pipes at each index of the 2D pipe array
    for (int i = 0; i < k; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return EXIT_FAILURE;
        }
    }

    // Spawn processes
    for (int i = 0; i < k; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            node_process(i, k, pipes[i], pipes[(i + 1) % k]);
            exit(0);
        }
    }

    // we will want to be able to enter a message more than once.
    while (1){
        // get the input to send in the apple.
        int nodeToSendMessageTo;
        puts("Enter the node number you want to send a message to:");
        scanf("%d", &nodeToSendMessageTo); 
        char messageToSend[MSG_SIZE];
        printf("Enter the message you want to send. Maximum message size is: %d", MSG_SIZE);
        // we have to use fgets since we want to be able to take multi word messages
        fgets(messageToSend, MSG_SIZE, stdin);
        // got the input for the apple, now package apple and write to the first pipe.
        message_t init_msg = {0, nodeToSendMessageTo, messageToSend};
        
        // Write to the first node
        write(pipes[0][1], &init_msg, sizeof(init_msg));

    }

    // Wait for all child processes
    for (int i = 0; i < k; i++) wait(NULL);

    return 0;
}