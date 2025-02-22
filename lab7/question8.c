#include <sys/resource.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    struct rlimit rlimitvalue;
    getrlimit(RLIMIT_FSIZE, &rlimitvalue);
    printf("maximum file size: %llu\n", rlimitvalue.rlim_max);
    return 0;
}
