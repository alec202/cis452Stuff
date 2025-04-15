#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>

// ANSI color codes for output
#define COLOR_RED "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_MAGENTA "\033[0;35m"
#define COLOR_CYAN "\033[0;36m"

// Enum for recipe types 
typedef enum {
    COOKIES,
    PANCAKES,
    PIZZA_DOUGH,
    SOFT_PRETZELS,
    CINNAMON_ROLLS
} RecipeType;

// Pantry structure 
typedef struct {
    int flour;
    int sugar;
    int yeast;
    int baking_soda;
    int salt;
    int cinnamon;
} Pantry;

// Fridge structure 
typedef struct {
    int eggs;
    int milk;
    int butter;
} Fridge;

// Kitchen structure 
typedef struct {
    int bowls;
    int spoons;
    int mixers;
} Kitchen;

// RecipeCounter structure for assigning recipes
typedef struct {
    int next_recipe; // Counter to cycle through recipes
} RecipeCounter;

// BakerResources structure 
typedef struct {
    int flour;
    int sugar;
    int yeast;
    int cinnamon;
    int baking_soda;
    int salt;
    int eggs;
    int milk;
    int butter;
    int has_bowl;
    int has_spoon;
    int has_mixer;
} BakerResources;

// BakerData structure 
typedef struct {
    long baker_id;              // Unique ID for the baker
    RecipeType recipe_type;     // Assigned recipe
    Pantry* pantry_ptr;         // Pointer to shared pantry
    Fridge* fridge_ptr;         // Pointer to shared fridge
    Kitchen* kitchen_ptr;       // Pointer to shared kitchen
    RecipeCounter* counter_ptr; // Pointer to shared recipe counter
    int pantry_semid;           // Semaphore ID for pantry access
    int fridge_semid;           // Semaphore ID for fridge access
    int kitchen_semid;          // Semaphore ID for kitchen resources
    int recipe_semid;           // Semaphore ID for recipe counter
    BakerResources* resources;  // Pointer to baker's resources
} BakerData;

// Array of ANSI color codes
const char* COLORS[] = {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN
};
const int NUM_COLORS = 6;
const char* RESET_COLOR = "\033[0m";

// Recipe structures defining pantry and fridge item counts
typedef struct {
    int num_of_items_in_pantry;
    int num_of_items_in_fridge;
} cookies;

typedef struct {
    int num_of_items_in_pantry;
    int num_of_items_in_fridge;
} pancakes;

typedef struct {
    int num_of_items_in_pantry;
    int num_of_items_in_fridge;
} homemadePizzaDough;

typedef struct {
    int num_of_items_in_pantry;
    int num_of_items_in_fridge;
} softPretzels;

typedef struct {
    int num_of_items_in_pantry;
    int num_of_items_in_fridge;
} cinnamonRolls;

// Initialize recipe requirements 
cookies cookiesRecipe = {2, 2};                   // Flour, sugar, eggs, butter
pancakes pancakesRecipe = {4, 2};                 // Flour, sugar, baking soda, salt, eggs, milk
homemadePizzaDough pizzaDoughRecipe = {3, 0};     // Flour, yeast, salt
softPretzels softPretzelsRecipe = {5, 1};         // Flour, sugar, yeast, salt, baking soda, butter
cinnamonRolls cinnamonRollsRecipe = {4, 2};       // Flour, sugar, yeast, cinnamon, eggs, butter

// Array of recipe names for printing
const char* recipe_names[] = {
    "Cookies",
    "Pancakes",
    "Pizza Dough",
    "Soft Pretzels",
    "Cinnamon Rolls"
};

// Decrement semaphore to acquire resource 
void sem_wait(int semid, int sem_num) {
    struct sembuf op = {sem_num, -1, 0}; // Decrease semaphore by 1
    if (semop(semid, &op, 1) < 0) {
        perror("sem_wait failed");
    }
}

// Increment semaphore to release resource
void sem_signal(int semid, int sem_num) {
    struct sembuf op = {sem_num, 1, 0}; // Increase semaphore by 1
    if (semop(semid, &op, 1) < 0) {
        perror("sem_signal failed");
    }
}

// Print baker's current state with colored output
void print_baker_state(long baker_id, BakerResources* resources, RecipeType recipe_type) {
    const char* color = COLORS[baker_id % NUM_COLORS]; // Select color based on baker ID
    printf("%sBaker %ld (Recipe: %s) state: Flour=%d, Sugar=%d, Yeast=%d, BakingSoda=%d, "
           "Salt=%d, Cinnamon=%d, Eggs=%d, Milk=%d, Butter=%d, "
           "Bowl=%d, Spoon=%d, Mixer=%d%s\n",
           color, baker_id, recipe_names[recipe_type],
           resources->flour, resources->sugar, resources->yeast,
           resources->baking_soda, resources->salt, resources->cinnamon,
           resources->eggs, resources->milk, resources->butter,
           resources->has_bowl, resources->has_spoon, resources->has_mixer, RESET_COLOR);
}

// Thread function for each baker
void* baker_function(void* arg) {
    BakerData* data = (BakerData*)arg;
    long baker_id = data->baker_id;
    Pantry* pantry_ptr = data->pantry_ptr;
    Fridge* fridge_ptr = data->fridge_ptr;
    Kitchen* kitchen_ptr = data->kitchen_ptr;
    RecipeCounter* counter_ptr = data->counter_ptr;
    int pantry_semid = data->pantry_semid;
    int fridge_semid = data->fridge_semid;
    int kitchen_semid = data->kitchen_semid;
    int recipe_semid = data->recipe_semid;
    BakerResources* resources = data->resources;

    // Select recipe using shared counter
    sem_wait(recipe_semid, 0); 
    // type cast that converts this integer into a value of the RecipeType enum,
    RecipeType recipe_type = (RecipeType)(counter_ptr->next_recipe % 5); 
    counter_ptr->next_recipe++; // Increment for next baker
    sem_signal(recipe_semid, 0); // Unlock recipe counter

    data->recipe_type = recipe_type; // Store for printing state
    printf("Baker %ld assigned recipe: %s\n", baker_id, recipe_names[recipe_type]);

    // Collect ingredients based on assigned recipe
    switch (recipe_type) {
        case COOKIES:
            // Cookies: 1 flour, 1 sugar, 1 egg, 1 butter
            sem_wait(pantry_semid, 0); // Enter pantry
            if (pantry_ptr->flour > 0) {
                pantry_ptr->flour--;
                resources->flour++;
                printf("Baker %ld took 1 flour from pantry. Pantry flour left: %d\n",
                       baker_id, pantry_ptr->flour);
            } else {
                printf("Baker %ld found no flour in pantry.\n", baker_id);
            }
            sem_signal(pantry_semid, 0); // Exit pantry
            print_baker_state(baker_id, resources, recipe_type);

            sem_wait(pantry_semid, 0);
            if (pantry_ptr->sugar > 0) {
                pantry_ptr->sugar--;
                resources->sugar++;
                printf("Baker %ld took 1 sugar from pantry. Pantry sugar left: %d\n",
                       baker_id, pantry_ptr->sugar);
            } else {
                printf("Baker %ld found no sugar in pantry.\n", baker_id);
            }
            sem_signal(pantry_semid, 0);
            print_baker_state(baker_id, resources, recipe_type);

            sem_wait(fridge_semid, 0); // Enter fridge
            if (fridge_ptr->eggs > 0) {
                fridge_ptr->eggs--;
                resources->eggs++;
                printf("Baker %ld took 1 egg from fridge. Fridge eggs left: %d\n",
                       baker_id, fridge_ptr->eggs);
            } else {
                printf("Baker %ld found no eggs in fridge.\n", baker_id);
            }
            sem_signal(fridge_semid, 0); // Exit fridge
            print_baker_state(baker_id, resources, recipe_type);

            sem_wait(fridge_semid, 0);
            if (fridge_ptr->butter > 0) {
                fridge_ptr->butter--;
                resources->butter++;
                printf("Baker %ld took 1 butter from fridge. Fridge butter left: %d\n",
                       baker_id, fridge_ptr->butter);
            } else {
                printf("Baker %ld found no butter in fridge.\n", baker_id);
            }
            sem_signal(fridge_semid, 0);
            print_baker_state(baker_id, resources, recipe_type);
            break;

        case PANCAKES:
            
        case PIZZA_DOUGH:
            
        case SOFT_PRETZELS:
            
        case CINNAMON_ROLLS:
           
    }

    // Collect kitchen resources
    // All recipes need a bowl and mixer
    sem_wait(kitchen_semid, 0); // Acquire bowl
    if (kitchen_ptr->bowls > 0) {
        kitchen_ptr->bowls--;
        resources->has_bowl = 1;
        printf("Baker %ld took a bowl from kitchen. Bowls left: %d\n",
               baker_id, kitchen_ptr->bowls);
    } else {
        printf("Baker %ld found no bowls in kitchen.\n", baker_id);
        resources->has_bowl = 0;
    }
    print_baker_state(baker_id, resources, recipe_type);

    sem_wait(kitchen_semid, 2); // Acquire mixer
    if (kitchen_ptr->mixers > 0) {
        kitchen_ptr->mixers--;
        resources->has_mixer = 1;
        printf("Baker %ld took a mixer from kitchen. Mixers left: %d\n",
               baker_id, kitchen_ptr->mixers);
    } else {
        printf("Baker %ld found no mixers in kitchen.\n", baker_id);
        resources->has_mixer = 0;
    }
    print_baker_state(baker_id, resources, recipe_type);

    //recipe completion
    printf("Baker %ld completed %s\n", baker_id, recipe_names[recipe_type]);

    // Return kitchen resources to allow other bakers to use them
    if (resources->has_bowl) {
        sem_signal(kitchen_semid, 0); // Release bowl
        kitchen_ptr->bowls++;
    }

    if (resources->has_mixer) {
        sem_signal(kitchen_semid, 2); // Release mixer
        kitchen_ptr->mixers++;
    }

    // Free thread-specific memory
    free(data->resources);
    free(data);
    return NULL;
}

// Main function to set up shared memory, semaphores, and threads
int main() {
    int num_bakers;

    // Prompt user for number of bakers
    printf("Enter the number of bakers to create: ");
    if (scanf("%d", &num_bakers) != 1 || num_bakers <= 0) {
        printf("Please enter a positive number.\n");
        return 1;
    }

    // Allocate memory for baker threads
    pthread_t* bakers = malloc(num_bakers * sizeof(pthread_t));
    if (bakers == NULL) {
        perror("Failed to allocate memory for bakers");
        return 1;
    }

    // Create semaphore for pantry 
    int pantry_semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (pantry_semid < 0) {
        perror("Pantry semget failed");
        free(bakers);
        return 1;
    }
    if (semctl(pantry_semid, 0, SETVAL, 1) < 0) {
        perror("Pantry semctl SETVAL failed");
        free(bakers);
        return 1;
    }

    // Create semaphore for fridge 
    int fridge_semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (fridge_semid < 0) {
        perror("Fridge semget failed");
        semctl(pantry_semid, 0, IPC_RMID);
        free(bakers);
        return 1;
    }
    if (semctl(fridge_semid, 0, SETVAL, 1) < 0) {
        perror("Fridge semctl SETVAL failed");
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(fridge_semid, 0, IPC_RMID);
        free(bakers);
        return 1;
    }

    // Create semaphore for kitchen
    int kitchen_semid = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);
    if (kitchen_semid < 0) {
        perror("Kitchen semget failed");
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(fridge_semid, 0, IPC_RMID);
        free(bakers);
        return 1;
    }
    if (semctl(kitchen_semid, 0, SETVAL, 3) < 0) { // 3 bowls
        perror("Kitchen semctl SETVAL bowls failed");
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(fridge_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        free(bakers);
        return 1;
    }
    if (semctl(kitchen_semid, 1, SETVAL, 5) < 0) { // 5 spoons
        perror("Kitchen semctl SETVAL spoons failed");
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(fridge_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        free(bakers);
        return 1;
    }
    if (semctl(kitchen_semid, 2, SETVAL, 2) < 0) { // 2 mixers
        perror("Kitchen semctl SETVAL mixers failed");
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(fridge_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        free(bakers);
        return 1;
    }

    // Create semaphore for recipe counter
    int recipe_semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (recipe_semid < 0) {
        perror("Recipe semget failed");
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(fridge_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        free(bakers);
        return 1;
    }
    if (semctl(recipe_semid, 0, SETVAL, 1) < 0) {
        perror("Recipe semctl SETVAL failed");
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(fridge_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        semctl(recipe_semid, 0, IPC_RMID);
        free(bakers);
        return 1;
    }

    // Create shared memory for pantry
    int pantry_shmid = shmget(IPC_PRIVATE, sizeof(Pantry), IPC_CREAT | 0666);
    if (pantry_shmid < 0) {
        perror("Pantry shmget failed");
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(fridge_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        semctl(recipe_semid, 0, IPC_RMID);
        free(bakers);
        return 1;
    }
    Pantry* pantry_ptr = (Pantry*)shmat(pantry_shmid, NULL, 0);
    if (pantry_ptr == (void*)-1) {
        perror("Pantry shmat failed");
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(fridge_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        semctl(recipe_semid, 0, IPC_RMID);
        shmctl(pantry_shmid, IPC_RMID, NULL);
        free(bakers);
        return 1;
    }
    // Initialize pantry with 10 
    pantry_ptr->flour = 10;
    pantry_ptr->sugar = 10;
    pantry_ptr->yeast = 10;
    pantry_ptr->baking_soda = 10;
    pantry_ptr->salt = 10;
    pantry_ptr->cinnamon = 10;

    // Create shared memory for fridge
    int fridge_shmid = shmget(IPC_PRIVATE, sizeof(Fridge), IPC_CREAT | 0666);
    if (fridge_shmid < 0) {
        perror("Fridge shmget failed");
        shmdt(pantry_ptr);
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(fridge_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        semctl(recipe_semid, 0, IPC_RMID);
        shmctl(pantry_shmid, IPC_RMID, NULL);
        free(bakers);
        return 1;
    }
    Fridge* fridge_ptr = (Fridge*)shmat(fridge_shmid, NULL, 0);
    if (fridge_ptr == (void*)-1) {
        perror("Fridge shmat failed");
        shmdt(pantry_ptr);
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(fridge_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        semctl(recipe_semid, 0, IPC_RMID);
        shmctl(pantry_shmid, IPC_RMID, NULL);
        shmctl(fridge_shmid, IPC_RMID, NULL);
        free(bakers);
        return 1;
    }
    // Initialize fridge with 10 units of each ingredient
    fridge_ptr->eggs = 10;
    fridge_ptr->milk = 10;
    fridge_ptr->butter = 10;

    // Create shared memory for kitchen
    int kitchen_shmid = shmget(IPC_PRIVATE, sizeof(Kitchen), IPC_CREAT | 0666);
    if (kitchen_shmid < 0) {
        perror("Kitchen shmget failed");
        shmdt(pantry_ptr);
        shmdt(fridge_ptr);
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(fridge_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        semctl(recipe_semid, 0, IPC_RMID);
        shmctl(pantry_shmid, IPC_RMID, NULL);
        shmctl(fridge_shmid, IPC_RMID, NULL);
        free(bakers);
        return 1;
    }
    Kitchen* kitchen_ptr = (Kitchen*)shmat(kitchen_shmid, NULL, 0);
    if (kitchen_ptr == (void*)-1) {
        perror("Kitchen shmat failed");
        shmdt(pantry_ptr);
        shmdt(fridge_ptr);
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(fridge_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        semctl(recipe_semid, 0, IPC_RMID);
        shmctl(pantry_shmid, IPC_RMID, NULL);
        shmctl(fridge_shmid, IPC_RMID, NULL);
        shmctl(kitchen_shmid, IPC_RMID, NULL);
        free(bakers);
        return 1;
    }
    // Initialize kitchen resources
    kitchen_ptr->bowls = 3;
    kitchen_ptr->spoons = 5;
    kitchen_ptr->mixers = 2;

    // Create shared memory for recipe counter
    int counter_shmid = shmget(IPC_PRIVATE, sizeof(RecipeCounter), IPC_CREAT | 0666);
    if (counter_shmid < 0) {
        perror("Counter shmget failed");
        shmdt(pantry_ptr);
        shmdt(fridge_ptr);
        shmdt(kitchen_ptr);
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(fridge_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        semctl(recipe_semid, 0, IPC_RMID);
        shmctl(pantry_shmid, IPC_RMID, NULL);
        shmctl(fridge_shmid, IPC_RMID, NULL);
        shmctl(kitchen_shmid, IPC_RMID, NULL);
        free(bakers);
        return 1;
    }
    RecipeCounter* counter_ptr = (RecipeCounter*)shmat(counter_shmid, NULL, 0);
    if (counter_ptr == (void*)-1) {
        perror("Counter shmat failed");
        shmdt(pantry_ptr);
        shmdt(fridge_ptr);
        shmdt(kitchen_ptr);
        semctl(pantry_semid, 0, IPC_RMID);
        semctl(fridge_semid, 0, IPC_RMID);
        semctl(kitchen_semid, 0, IPC_RMID);
        semctl(recipe_semid, 0, IPC_RMID);
        shmctl(pantry_shmid, IPC_RMID, NULL);
        shmctl(fridge_shmid, IPC_RMID, NULL);
        shmctl(kitchen_shmid, IPC_RMID, NULL);
        shmctl(counter_shmid, IPC_RMID, NULL);
        free(bakers);
        return 1;
    }
    // Initialize recipe counter
    counter_ptr->next_recipe = 0;

    // Create baker threads
    for (long i = 0; i < num_bakers; i++) {
        // Allocate memory for thread data
        BakerData* bdata = malloc(sizeof(BakerData));
        if (bdata == NULL) {
            perror("Failed to allocate BakerData");
            shmdt(pantry_ptr);
            shmdt(fridge_ptr);
            shmdt(kitchen_ptr);
            shmdt(counter_ptr);
            semctl(pantry_semid, 0, IPC_RMID);
            semctl(fridge_semid, 0, IPC_RMID);
            semctl(kitchen_semid, 0, IPC_RMID);
            semctl(recipe_semid, 0, IPC_RMID);
            shmctl(pantry_shmid, IPC_RMID, NULL);
            shmctl(fridge_shmid, IPC_RMID, NULL);
            shmctl(kitchen_shmid, IPC_RMID, NULL);
            shmctl(counter_shmid, IPC_RMID, NULL);
            free(bakers);
            return 1;
        }
        // Initialize thread data
        bdata->baker_id = i;
        bdata->recipe_type = 0; // Will be set in baker_function
        bdata->pantry_ptr = pantry_ptr;
        bdata->fridge_ptr = fridge_ptr;
        bdata->kitchen_ptr = kitchen_ptr;
        bdata->counter_ptr = counter_ptr;
        bdata->pantry_semid = pantry_semid;
        bdata->fridge_semid = fridge_semid;
        bdata->kitchen_semid = kitchen_semid;
        bdata->recipe_semid = recipe_semid;
        bdata->resources = malloc(sizeof(BakerResources));
        if (bdata->resources == NULL) {
            perror("Failed to allocate BakerResources");
            free(bdata);
            shmdt(pantry_ptr);
            shmdt(fridge_ptr);
            shmdt(kitchen_ptr);
            shmdt(counter_ptr);
            semctl(pantry_semid, 0, IPC_RMID);
            semctl(fridge_semid, 0, IPC_RMID);
            semctl(kitchen_semid, 0, IPC_RMID);
            semctl(recipe_semid, 0, IPC_RMID);
            shmctl(pantry_shmid, IPC_RMID, NULL);
            shmctl(fridge_shmid, IPC_RMID, NULL);
            shmctl(kitchen_shmid, IPC_RMID, NULL);
            shmctl(counter_shmid, IPC_RMID, NULL);
            free(bakers);
            return 1;
        }
        // Initialize baker's resources to zero
        bdata->resources->flour = 0;
        bdata->resources->sugar = 0;
        bdata->resources->yeast = 0;
        bdata->resources->cinnamon = 0;
        bdata->resources->baking_soda = 0;
        bdata->resources->salt = 0;
        bdata->resources->eggs = 0;
        bdata->resources->milk = 0;
        bdata->resources->butter = 0;
        bdata->resources->has_bowl = 0;
        bdata->resources->has_spoon = 0;
        bdata->resources->has_mixer = 0;

        // Create baker thread
        if (pthread_create(&bakers[i], NULL, baker_function, (void*)bdata) != 0) {
            perror("Baker creation failed");
            free(bdata->resources);
            free(bdata);
            shmdt(pantry_ptr);
            shmdt(fridge_ptr);
            shmdt(kitchen_ptr);
            shmdt(counter_ptr);
            semctl(pantry_semid, 0, IPC_RMID);
            semctl(fridge_semid, 0, IPC_RMID);
            semctl(kitchen_semid, 0, IPC_RMID);
            semctl(recipe_semid, 0, IPC_RMID);
            shmctl(pantry_shmid, IPC_RMID, NULL);
            shmctl(fridge_shmid, IPC_RMID, NULL);
            shmctl(kitchen_shmid, IPC_RMID, NULL);
            shmctl(counter_shmid, IPC_RMID, NULL);
            free(bakers);
            return 1;
        }
    }

    // Wait for all baker threads to complete
    for (int i = 0; i < num_bakers; i++) {
        pthread_join(bakers[i], NULL);
    }

    // Print final state of pantry, fridge, and kitchen
    printf("\nFinal state:\n");
    printf("Pantry: Flour=%d, Sugar=%d, Yeast=%d, Baking Soda=%d, Salt=%d, Cinnamon=%d\n",
           pantry_ptr->flour, pantry_ptr->sugar, pantry_ptr->yeast,
           pantry_ptr->baking_soda, pantry_ptr->salt, pantry_ptr->cinnamon);
    printf("Fridge: Eggs=%d, Milk=%d, Butter=%d\n",
           fridge_ptr->eggs, fridge_ptr->milk, fridge_ptr->butter);
    printf("Kitchen: Bowls=%d, Spoons=%d, Mixers=%d\n",
           kitchen_ptr->bowls, kitchen_ptr->spoons, kitchen_ptr->mixers);

    // Clean up resources
    semctl(pantry_semid, 0, IPC_RMID);   // Remove pantry semaphore
    semctl(fridge_semid, 0, IPC_RMID);   // Remove fridge semaphore
    semctl(kitchen_semid, 0, IPC_RMID);  // Remove kitchen semaphore
    semctl(recipe_semid, 0, IPC_RMID);   // Remove recipe semaphore
    shmdt(pantry_ptr);                   // Detach pantry shared memory
    shmdt(fridge_ptr);                   // Detach fridge shared memory
    shmdt(kitchen_ptr);                  // Detach kitchen shared memory
    shmdt(counter_ptr);                  // Detach counter shared memory
    shmctl(pantry_shmid, IPC_RMID, NULL);  // Remove pantry shared memory
    shmctl(fridge_shmid, IPC_RMID, NULL);  // Remove fridge shared memory
    shmctl(kitchen_shmid, IPC_RMID, NULL); // Remove kitchen shared memory
    shmctl(counter_shmid, IPC_RMID, NULL); // Remove counter shared memory
    free(bakers);                        // Free thread array
    printf("Cleanup complete.\n");

    return 0;
}