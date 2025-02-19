#include <limits.h>
#include <stdio.h>
#include <semaphore.h>

int main() {
    printf("Maximum value of a semaphore: %d\n", SEM_VALUE_MAX);
    return 0;
}
