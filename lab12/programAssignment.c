#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>


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

    while ((entryPtr = readdir(dirPtr))){
        if (stat(entryPtr->d_name, &fileStatBuff) < 0){
            perror("Program error with the stat command:  ");
            exit(1);
        } 
        printf("%-20s\tThe user ID is: %u, the group ID is: %u. The inode value is: %llu\n", entryPtr->d_name, fileStatBuff.st_uid, fileStatBuff.st_gid, fileStatBuff.st_ino);
    }
    closedir(dirPtr);    
    return 0;
}

