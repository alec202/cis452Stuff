#include <stdio.h>
#include <pthread.h>

// Define color codes
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;35m"
#define RESET "\033[0m"

// Structure to pass thread ID and color code
typedef struct {
    int thread_id;
    char* color_code;
} thread_data;

// Function to be executed by each thread
void* print_message(void* arg) {
    thread_data* data = (thread_data*)arg;
    printf("%sThread %d: This is a message in color.%s\n", data->color_code, data->thread_id, RESET);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[5];
    thread_data thread_args[5];
    char* colors[] = {RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN};

    // Create and run threads
    for (int i = 0; i < 5; i++) {
        thread_args[i].thread_id = i + 1;
        thread_args[i].color_code = colors[i % 5];
        pthread_create(&threads[i], NULL, print_message, (void*)&thread_args[i]);
    }

    // Wait for threads to finish
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
