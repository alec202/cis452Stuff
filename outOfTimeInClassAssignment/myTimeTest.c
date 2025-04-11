#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <utime.h>
#include <errno.h>

const char *filename = "myTestFile.txt";
int main()
{
    struct stat myStat;
    if (stat("./myTestFile.txt", &myStat) < 0){
        perror("stat");
        return 1;
    }

    printf("Modified \t%s", ctime(&myStat.st_mtime));
    printf("Accessed \t%s", ctime(&myStat.st_atime));
    printf("Changed \t%s", ctime(&myStat.st_ctime));

    struct utimbuf {
        time_t actime;
        time_t modtime;
    } newTimes;

    newTimes.actime = time(NULL) - 86400 * 3;
    newTimes.modtime = time(NULL) - 86400 * 3;
    if (utime("./myTestFile.txt", &newTimes) != 0){
        perror("utime Error");
        return 1;
    }
    puts("File time was updated to be before April 9th.");

    if (stat("./myTestFile.txt", &myStat) < 0){
        perror("stat 2");
        return 1;
    }
    printf("Modified \t%s", ctime(&myStat.st_mtime));
    printf("Accessed \t%s", ctime(&myStat.st_atime));
    printf("Changed \t%s", ctime(&myStat.st_ctime));

    puts("File time was updated to be before April 8th, use ls -l command to verify.");
    return 0;
}
