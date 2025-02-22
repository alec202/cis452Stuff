#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    long pages = sysconf(_SC_PHYS_PAGES);
    printf("The maximum number of physical pages is: %lu \n", pages);
    return 0;
}

