#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#define SIZE 30
int main(int argc, char *argv[])
{
    struct flock fileLock;
    int fd;
    char myBuffer[SIZE];
    if (argc < 2)
    {
        printf("usage: filename\n");
        exit(1);
    }
    if ((fd = open(argv[1], O_RDWR)) < 0)
    {
        perror("there is");
        exit(1);
    }
    
    fileLock.l_type = F_WRLCK;
    fileLock.l_whence = SEEK_SET;
    fileLock.l_start = 0;
    fileLock.l_len = 0;
    if (fcntl(fd, F_SETLKW, &fileLock) < 0)
    {
        perror("Unable to set file lock");
        exit(1);
    }
    ssize_t bytesRead = read(fd, myBuffer, SIZE);
    if (bytesRead < 0) {
        perror("read failed");
        close(fd);
        exit(1);
    }

    for (int i = 0; i < bytesRead; i++) {
        if (myBuffer[i] == '\n') {
            myBuffer[i] = '\0'; // terminate the first line
            break;
        }
    }
    printf("First line: %s\n", myBuffer);

    sleep(10);
    fileLock.l_type = F_UNLCK;  // Set lock type to unlock
    fileLock.l_whence = SEEK_SET; // Offset relative to the beginning of the file
    fileLock.l_start = 0;        // Start offset at 0
    fileLock.l_len = 0;          // Unlock the entire file (0 means to EOF)

    if (fcntl(fd, F_SETLK, &fileLock) == -1) {
        perror("fcntl (unlock) Failed");
        return -1; // Error unlocking the file
    }

    close(fd);
    return 0;
}
