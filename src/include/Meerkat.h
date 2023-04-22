#ifndef MEERKAT_H
#define MEERKAT_H

#define _GNU_SOURCE

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include "ProducerConsumer.h"
#include "Message.h"
#include "Animal.h"

extern pthread_mutex_t mutex_stdout;

#define MEERKAT_MIN_SLEEP_TIME 6
#define MEERKAT_MAX_SLEEP_TIME 10
#define MEERKAT_MIN_EAT 1
#define MEERKAT_MAX_EAT 2

typedef struct {
    Animal animal;
} Meerkat;

typedef struct {
    Meerkat *meerkat_instance;
    sem_t *feeder_ammount;
    sem_t *feeder_ammount_spaces_empty;
    ProducerConsumerBuffer *incoming_communication;
    ProducerConsumerBuffer *veterinarian_comunication;
} MeerkatArgs;

void *Meerkat_run_routine(void *args);

void Meerkat_handle_clock(MeerkatArgs *args);

void Meerkat_eat(MeerkatArgs *args);

void Meerkat_sleep(MeerkatArgs *args);

void Meerkat_show(MeerkatArgs *args);

void Meerkat_change_state(MeerkatArgs *args);

void Meerkat_print_log(MeerkatArgs *args, char *message);

#endif