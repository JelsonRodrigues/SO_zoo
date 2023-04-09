#ifndef OSTRICH_H
#define OSTRICH_H

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include "ProducerConsumer.h"
#include "Message.h"
#include "Animal.h"

extern pthread_mutex_t mutex_stdout;

#define OSTRICH_MIN_SLEEP_TIME 4
#define OSTRICH_MAX_SLEEP_TIME 8
#define OSTRICH_MIN_EAT 2
#define OSTRICH_MAX_EAT 4

typedef struct {
    Animal animal;
} Ostrich;

typedef struct {
    Ostrich *ostrich_instance;
    sem_t *feeder_ammount;
    sem_t* ostrich_food_needed;
    sem_t *animals_waiting_or_eating;
    ProducerConsumerBuffer *incomming_communication;
} OstrichArgs;

void *Ostrich_run_routine(void *args);

void Ostrich_handle_clock(OstrichArgs *args);

void Ostrich_eat(OstrichArgs *args);

void Ostrich_sleep(OstrichArgs *args);

void Ostrich_show(OstrichArgs *args);
#endif