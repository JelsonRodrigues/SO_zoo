#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include "src/include/ProducerConsumer.h"
#include "src/include/Zoo.h"

// This mutex is for the stdout, so only one thread writes at time
pthread_mutex_t mutex_stdout = PTHREAD_MUTEX_INITIALIZER;

int main() {
    // Setting the random number generator seed to 0, so the result is always the same
    srand(0);
    
    // Initialize zoo
    Zoo zoo;
    
    // Launch the zoo thread
    pthread_t zoo_tid;
    pthread_create(&zoo_tid, NULL, zoo_run_routine, (void *) &zoo);
    
    // Wait the zoo simulation to finish
    pthread_join(zoo_tid, NULL);

    // Free zoo
    free_zoo(&zoo);

    return EXIT_SUCCESS;
}