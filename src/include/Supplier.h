#ifndef SUPPLIER_H
#define SUPPLIER_H

#include <stdint.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include "ProducerConsumer.h"
#include "Message.h"

#ifndef MAX_STOCKED_FOOD
#define MAX_STOCKED_FOOD 30
#endif

extern pthread_mutex_t mutex_stdout;

typedef struct {
    int32_t id;

    uint32_t meerkat_food_supplied;
    uint32_t lion_food_supplied;
    uint32_t ostrich_food_supplied;
} Supplier;
    
typedef struct {
    Supplier *supplier_object;
    sem_t *meerkat_food_storage;
    sem_t *lion_food_storage;
    sem_t *ostrich_food_storage;

    ProducerConsumerBuffer *incoming_communication;
} SupplierArgs;

void *Supplier_run_routine(void *args);

void supply(SupplierArgs *args);

#endif