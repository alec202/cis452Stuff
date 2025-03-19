#include <stdio.h>
#include <stdlib.h>

// Initialized Data
int global_init = 22;

// Uninitialized Data
int global_uninit;

void recursive_function(int depth) {
    int local_var = depth;
    if (depth > 0) {
        printf("Stack address (local_var at depth %d): %p\n", depth, &local_var);
        recursive_function(depth - 1);
    }
}

int main() {
    // Program Text (code is here)
    printf("Text segment (main function): %p\n", main);

    // Initialized Data
    printf("Initialized data (global_init): %p\n", &global_init);

    // Uninitialized Data
    printf("Uninitialized data (global_uninit): %p\n", &global_uninit);

    // Heap
    int *heap_var = malloc(sizeof(int));
    *heap_var = 100;
    printf("Heap (malloc'd variable): %p\n", heap_var);

    // Stack
    int stack_var = 5;
    printf("Stack (stack_var): %p\n", &stack_var);
    recursive_function(3); // Show stack growth downward

    // Shared Library (example: printf)
    printf("Shared library (printf function): %p\n", printf);

    free(heap_var);
    return 0;
}