#include <stdio.h>
#include <unistd.h>

int main() {
	//The sysconf() with the POSIX variable _SC_CHILD_MAX
	//CHILD_MAX - _SC_CHILD_MAX
              //The maximum number of simultaneous processes per user ID.
              
    long max_procs = sysconf(_SC_CHILD_MAX);
    if (max_procs == -1) {
        perror("sysconf failed");
        return 1;
    }
    printf("Maximum number of processes per user (sysconf): %ld\n", max_procs);
    return 0;
}