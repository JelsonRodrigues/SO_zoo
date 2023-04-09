#ifndef ZOO_H
#define ZOO_H

#include <stdint.h>
#include "Vector.h"
#include "ProducerConsumer.h"
#include "Message.h"

#define MAX_STOCKED_FOOD 30
#define MAX_AVAILABLE_FOOD 10

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

    // Quantity of animals eating or waiting to eat
    sem_t animals_waiting_or_eating;
    sem_t meerkat_food_needed;
    sem_t ostrich_food_needed;
    sem_t lion_food_needed;

    // Quantity of available slots
    sem_t meerkat_food_slots_available;
    sem_t ostrich_food_slots_available;
    sem_t lion_food_slots_available;

    // The order for the veterinaries to serve the species
    Vector animals_serving_order;

    // Vector of communication with each animal
    Vector ostrichs;
    Vector lions;
    Vector meerkats;

    Vector veterinarians;
    Vector supplier;
    
} Zoo;

void increment_one_hour();

#endif