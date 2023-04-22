#ifndef ZOO_H
#define ZOO_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "Ostrich.h"
#include "Lion.h"
#include "Meerkat.h"
#include "Veterinarian.h"
#include "Vector.h"
#include "Supplier.h"
#include "ProducerConsumer.h"
#include "Message.h"

#define MAX_STOCKED_FOOD 30
#define MAX_AVAILABLE_FOOD 10
#define MAXIMUM_MESSAGES 32

#define LIONS 4
#define OSTRICHS 7
#define MEERKATS 10
#define VETERINARIANS 2

extern pthread_mutex_t mutex_stdout;

typedef struct {
    uint32_t time_elapsed;

    // Zoo entire food storage
    sem_t meerkat_food_storage;
    sem_t lion_food_storage;
    sem_t ostrich_food_storage;
    
    // Quantity of food in each eater
    sem_t meerkat_food_filled;
    sem_t ostrich_food_filled;
    sem_t lion_food_filled;

    // Quantity of available slots
    sem_t meerkat_food_slots_available;
    sem_t ostrich_food_slots_available;
    sem_t lion_food_slots_available;

    // Vector of communication with each actor
    Vector ostrichs_com;
    Vector lions_com;
    Vector meerkats_com;
    ProducerConsumerBuffer veterinarians_com;
    ProducerConsumerBuffer supplier_com;

    // A vector with the actors of the zoo
    Vector ostrichs;
    Vector lions;
    Vector meerkats;
    Vector veterinarians;
    Supplier supplier;

    // A vector with the pthread_t of each thread spawned by the zoo
    Vector threads;
} Zoo;

void create_zoo(Zoo *zoo);

void instantiate_actors(Zoo *zoo);

void semaphore_init(Zoo *zoo);

void spawn_threads(Zoo *zoo);

void increment_one_hour(Zoo *zoo);

void terminate_all_threads(Zoo *zoo);

void *zoo_run_routine(void *args);

void print_zoo_stats(Zoo *zoo);

void free_zoo(Zoo *zoo);

#endif