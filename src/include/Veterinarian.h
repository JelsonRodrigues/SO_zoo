#ifndef VETERINARIAN_H
#define VETERINARIAN_H

#include <stdint.h>
#include <pthread.h>
#include <stdio.h>
#include "ProducerConsumer.h"
#include "Message.h"
#include "Zoo.h"

extern pthread_mutex_t mutex_stdout;

typedef struct {
    int32_t id;
} Veterinarian;

typedef struct {
    Veterinarian *vet_instance;
    sem_t *meerkat_food_storage;
    sem_t *lion_food_storage;
    sem_t *ostrich_food_storage;
    sem_t *meerkat_food_filled;
    sem_t *ostrich_food_filled;
    sem_t *lion_food_filled;
    sem_t *meerkat_food_slots_available;
    sem_t *ostrich_food_slots_available;
    sem_t *lion_food_slots_available;
    ProducerConsumerBuffer *supplier_communication;
    ProducerConsumerBuffer *incoming_communication;
} VeterinarianArgs;

void *Veterinarian_run_routine(void *args);

void Veterinarian_handle_animal_eat_message(VeterinarianArgs *args, AnimalEat message);

void Veterinarian_fill_lion_eater(VeterinarianArgs *args, uint32_t ammount);

void Veterinarian_fill_ostrich_eater(VeterinarianArgs *args, uint32_t ammount);

void Veterinarian_fill_meerkat_eater(VeterinarianArgs *args, uint32_t ammount);

void Veterinarian_request_stock_refill(VeterinarianArgs *args);

#endif