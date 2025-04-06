#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define COLOR_RED "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_MAGENTA "\033[0;35m"
#define COLOR_CYAN "\033[0;35m"

// Pantry structure in shared memory
typedef struct {
    int flour;        
    int sugar;        
    int yeast;        
    int baking_soda;  
    int salt;         
    int cinnamon;     
} Pantry;

// Kitchen structure in shared memory
typedef struct {
    int bowls;        
    int spoons;   
    int mixer;
} Kitchen;

// Baker's local resource tracking
typedef struct {
    int flour;
    int sugar;
    int yeast;
    int baking_soda;
    int salt;
    int caramel;
    int eggs;
    int milk;
    int butter;
    int has_bowl;
    int has_spoon;
    int has_mixer
} BakerResources;

// Thread data structure
typedef struct {
    long baker_id;
    Pantry* pantry_ptr;
    Kitchen* kitchen_ptr;
    int pantry_semid;   // Semaphore for exclusive pantry access
    int kitchen_semid;  // Semaphore set for kitchen resources 
    BakerResources* resources;  // Local resource tracking
} BakerData;

void sem_wait(int semid, int sem_num) {
    struct sembuf op = {sem_num, -1, 0};  // Decrement semaphore sem_num (wait)
    semop(semid, &op, 1);                 
}

void sem_signal(int semid, int sem_num) {
    struct sembuf op = {sem_num, 1, 0};   // Increment semaphore sem_num (signal)
    semop(semid, &op, 1);                 
}
// ANSI color codes array
const char* COLORS[] = {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN
};

const int NUM_COLORS = 6;
const char* RESET_COLOR = "\033[0m"; // Reset to default color

// Helper function to print baker's state with color
void print_baker_state(long baker_id, BakerResources* resources) {
    const char* color = COLORS[baker_id % NUM_COLORS]; // Assign color based on baker_id
    printf("%sBaker %ld state: Flour=%d, Sugar=%d, Bowl=%d, Spoon=%d%s\n",
           color, baker_id, resources->flour, resources->sugar, 
           resources->has_bowl, resources->has_spoon, RESET_COLOR);
}

// Thread function: Baker behavior with state updates
void* baker_function(void* arg) {
    BakerData* data = (BakerData*)arg;
    long baker_id = data->baker_id;
    Pantry* pantry_ptr = data->pantry_ptr;
    Kitchen* kitchen_ptr = data->kitchen_ptr;
    int pantry_semid = data->pantry_semid;
    int kitchen_semid = data->kitchen_semid;
    BakerResources* resources = data->resources;

    //Get flour from pantry
    sem_wait(pantry_semid, 0);  // Enter pantry (exclusive)
    if (pantry_ptr->flour > 0) {
        pantry_ptr->flour--;
        resources->flour++;
        printf("Baker %ld took 1 flour from pantry. Pantry flour left: %d\n", 
               baker_id, pantry_ptr->flour);
    } else {
        printf("Baker %ld found no flour in pantry.\n", baker_id);
    }
    print_baker_state(baker_id, resources);
    sem_signal(pantry_semid, 0);  // Leave pantry

    //Get sugar from pantry
    sem_wait(pantry_semid, 0);  // Re-enter pantry
    if (pantry_ptr->sugar > 0) {
        pantry_ptr->sugar--;
        resources->sugar++;
        printf("Baker %ld took 1 sugar from pantry. Pantry sugar left: %d\n", 
               baker_id, pantry_ptr->sugar);
    } else {
        printf("Baker %ld found no sugar in pantry.\n", baker_id);
    }
    print_baker_state(baker_id, resources);
    sem_signal(pantry_semid, 0);  // Leave pantry

    //Get a bowl from kitchen
    sem_wait(kitchen_semid, 0);  // Wait for a bowl
    kitchen_ptr->bowls--;
    resources->has_bowl = 1;
    printf("Baker %ld took a bowl from kitchen. Bowls left: %d\n", 
           baker_id, kitchen_ptr->bowls);
    print_baker_state(baker_id, resources);

    //Get a spoon from kitchen
    sem_wait(kitchen_semid, 1);  // Wait for a spoon
    kitchen_ptr->spoons--;
    resources->has_spoon = 1;
    printf("Baker %ld took a spoon from kitchen. Spoons left: %d\n", 
           baker_id, kitchen_ptr->spoons);
    print_baker_state(baker_id, resources);

    //
    free(data->resources);
    free(data);
    return NULL;
}

int main() {
    int num_bakers;

    // Get number of bakers from user
    printf("Enter the number of bakers to create: ");
    scanf("%d", &num_bakers);

    //baker (thread) array
    pthread_t* bakers = malloc(num_bakers * sizeof(pthread_t));
    if (bakers == NULL) {
        perror("Failed to allocate memory for bakers");
        return 1;
    }

    //semaphore set for pantry (size 1)
    int pantry_semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (pantry_semid < 0) {
        perror("Pantry semget failed");
        free(bakers);
        return 1;
    }
    //Initialize the pantry semaphore
    semctl(pantry_semid, 0, SETVAL, 1);

    //semaphore set for kitchen 
    int kitchen_semid = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
    if (kitchen_semid < 0) {
        perror("Kitchen semget failed");
        semctl(pantry_semid, 0, IPC_RMID);
        free(bakers);
        return 1;
    }
    // Step b: Initialize kitchen semaphores 
    semctl(kitchen_semid, 0, SETVAL, 3);  // Bowls
    semctl(kitchen_semid, 1, SETVAL, 5);  // Spoons

    // Create pantry shared memory
    int pantry_shmid = shmget(IPC_PRIVATE, sizeof(Pantry), IPC_CREAT | 0666);
    if (pantry_shmid < 0) {
        perror("Pantry shmget failed");
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        free(bakers);
        return 1;
    }
    Pantry* pantry_ptr = (Pantry*)shmat(pantry_shmid, NULL, 0);
    if (pantry_ptr == (void*)-1) {
        perror("Pantry shmat failed");
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        free(bakers);
        return 1;
    }
    pantry_ptr->flour = 5;
    pantry_ptr->sugar = 5;
    pantry_ptr->yeast = 5;
    pantry_ptr->baking_soda = 5;
    pantry_ptr->salt = 5;
    pantry_ptr->cinnamon = 5;

    // Create kitchen shared memory
    int kitchen_shmid = shmget(IPC_PRIVATE, sizeof(Kitchen), IPC_CREAT | 0666);
    if (kitchen_shmid < 0) {
        perror("Kitchen shmget failed");
        shmdt(pantry_ptr);
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        free(bakers);
        return 1;
    }
    Kitchen* kitchen_ptr = (Kitchen*)shmat(kitchen_shmid, NULL, 0);
    if (kitchen_ptr == (void*)-1) {
        perror("Kitchen shmat failed");
        shmdt(pantry_ptr);
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        free(bakers);
        return 1;
    }
    kitchen_ptr->bowls = 3;   // 3 bowls available
    kitchen_ptr->spoons = 5;  // 5 spoons available

    // Create bakers (threads)
    for (long i = 0; i < num_bakers; i++) {
        BakerData* bdata = malloc(sizeof(BakerData));
        if (bdata == NULL) {
            perror("Failed to allocate BakerData");
            free(bakers);
            return 1;
        }
        bdata->baker_id = i;
        bdata->pantry_ptr = pantry_ptr;
        bdata->kitchen_ptr = kitchen_ptr;
        bdata->pantry_semid = pantry_semid;
        bdata->kitchen_semid = kitchen_semid;
        bdata->resources = malloc(sizeof(BakerResources));
        if (bdata->resources == NULL) {
            perror("Failed to allocate BakerResources");
            free(bdata);
            free(bakers);
            return 1;
        }
        bdata->resources->flour = 0;
        bdata->resources->sugar = 0;
        bdata->resources->has_bowl = 0;
        bdata->resources->has_spoon = 0;

        if (pthread_create(&bakers[i], NULL, baker_function, (void*)bdata) != 0) {
            perror("Baker creation failed");
            free(bdata->resources);
            free(bdata);
            free(bakers);
            return 1;
        }
    }

    // Wait for all bakers to complete
    for (int i = 0; i < num_bakers; i++) {
        pthread_join(bakers[i], NULL);
    }

    // Print final state
    printf("\nFinal state:\n");
    printf("Pantry: Flour=%d, Sugar=%d, Yeast=%d, Baking Soda=%d, Salt=%d, Cinnamon=%d\n",
           pantry_ptr->flour, pantry_ptr->sugar, pantry_ptr->yeast,
           pantry_ptr->baking_soda, pantry_ptr->salt, pantry_ptr->cinnamon);
    printf("Kitchen: Bowls=%d, Spoons=%d\n", kitchen_ptr->bowls, kitchen_ptr->spoons);

    
    semctl(pantry_semid, 0, IPC_RMID);   // Remove pantry semaphore
    semctl(kitchen_semid, 0, IPC_RMID);  // Remove kitchen semaphore

    // Cleanup shared memory
    shmdt(pantry_ptr);
    shmdt(kitchen_ptr);
    shmctl(pantry_shmid, IPC_RMID, NULL);
    shmctl(kitchen_shmid, IPC_RMID, NULL);
    free(bakers);
    printf("Cleanup complete.\n");

    return 0;
}
