#ifndef LION_H
#define LION_H

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include "ProducerConsumer.h"
#include "Message.h"
#include "Animal.h"

extern pthread_mutex_t mutex_stdout;

#define LION_MIN_SLEEP_TIME 8
#define LION_MAX_SLEEP_TIME 12
#define LION_MIN_EAT 2
#define LION_MAX_EAT 5

typedef struct {
    Animal animal;
} Lion;

typedef struct {
    Lion *lion_instance;
    sem_t *feeder_ammount;
    sem_t *feeder_ammount_spaces_empty;
    ProducerConsumerBuffer *incoming_communication;
    ProducerConsumerBuffer *veterinarian_comunication;
} LionArgs;

void *Lion_run_routine(void *args);

void Lion_handle_clock(LionArgs *args);

void Lion_eat(LionArgs *args);

void Lion_sleep(LionArgs *args);

void Lion_show(LionArgs *args);
#endif