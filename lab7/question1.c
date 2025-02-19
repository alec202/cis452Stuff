#include <limits.h>
#include <stdio.h>
#include <errno.h>

int main() {
  #ifdef SEM_NSEMS_MAX
    printf("SEM_NSEMS_MAX: %d\n", SEM_NSEMS_MAX);
  #else
    printf("SEM_NSEMS_MAX is not defined.\n");
    #ifdef _POSIX_SEM_NSEMS_MAX
        printf("_POSIX_SEM_NSEMS_MAX: %d\n", _POSIX_SEM_NSEMS_MAX);
    #else
        printf("_POSIX_SEM_NSEMS_MAX is not defined.\n");
    #endif
  #endif
  
  return 0;
}
