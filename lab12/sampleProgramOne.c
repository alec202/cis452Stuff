#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>


int main(int argc, char *argv[])
{
    struct stat statBuf;
    // checks to make sure we supplied a file name argument 
    if (argc < 2)
    {
        printf("Usage: argument (filename) expected\n");
        exit(1);
    }
    // makes sure the stat command didn't fail
    if (stat(argv[1], &statBuf) < 0)
    {
        perror("Program error with the stat command:  ");
        exit(1);
    }
    // print out information about the file type and permission
    printf("value is: %u\n", statBuf.st_mode);
    // print out the inode number
    printf("inode value is: %llu\n", statBuf.st_ino);
    
    // check if it's a directory
    if (S_ISDIR(statBuf.st_mode))
    {
        printf("The file is a directory.\n");
    }
    else
    {
        printf("The file is NOT a directory.\n");
    }

    return 0;
}

