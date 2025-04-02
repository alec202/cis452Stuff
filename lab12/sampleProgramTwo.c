#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
int main()
{
    DIR *dirPtr;
    struct dirent *entryPtr;
    dirPtr = opendir(".");
    struct stat fileStat;
    while ((entryPtr = readdir(dirPtr))){
        stat(entryPtr->d_name, &fileStat); 
        printf("%-20s size: %llu Bytes\n", entryPtr->d_name, fileStat.st_size);
    }
    closedir(dirPtr);
    return 0;
}
