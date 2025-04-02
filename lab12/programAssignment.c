#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>


int main(int argc, char *argv[])
{
    // checks to make sure we supplied a file name argument 
    if (argc < 2)
    {
        printf("Usage: argument (filename) expected\n");
        exit(1);
    }

    DIR *dirPtr;
    struct dirent *entryPtr;
    struct stat fileStatBuff;
    // opens directory stream for specified directory.
    char* directoryName = argv[1];
    dirPtr = opendir(directoryName);
    if (dirPtr == NULL) {
        // opendir failed – use errno to determine why
        fprintf(stderr, "Failed to open directory '%s': %s\n", directoryName, strerror(errno));
        return EXIT_FAILURE;
    }

    printf("specified directory is: %s\n", directoryName);
    while ((entryPtr = readdir(dirPtr))){
        // Create full path: directoryName + "/" + entry name
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", directoryName, entryPtr->d_name);
        if (stat(fullPath, &fileStatBuff) < 0){
            printf("in directory: %s\n", fullPath);
            perror("Program error with the stat command:  ");
            exit(1);
        } 
        printf("%-20s\tThe user ID is: %u, the group ID is: %u. The inode value is: %llu\n", fullPath, fileStatBuff.st_uid, fileStatBuff.st_gid, fileStatBuff.st_ino);
    }
    closedir(dirPtr);    
    return 0;
}

