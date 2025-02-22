#include <sys/resource.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    struct rlimit rlimitvalue;
    getrlimit(RLIMIT_NOFILE, &rlimitvalue);
    printf("maximum Number of Open files, soft limit: %llu\n", rlimitvalue.rlim_cur);
    return 0;
}
