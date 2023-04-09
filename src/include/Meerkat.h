#ifndef MEERKAT_H
#define MEERKAT_H

#include <unistd.h>
#include <pthread.h>
#include "ProducerConsumer.h"
#include "Animal.h"

extern pthread_mutex_t mutex_stdout;

#define MEERKAT_MAX_SLEEP_TIME 6
#define MEERKAT_MIN_SLEEP_TIME 10
#define MEERKAT_MIN_EAT 1
#define MEERKAT_MAX_EAT 2

typedef struct {
    Animal animal;
} Meerkat;

typedef struct {
    Meerkat *meerkat_instance;
    ProducerConsumerBuffer *meerkat_communication;
} MeerkatArgs;

void *Meerkat_run_routine(void *args);

void Meerkat_eat(Meerkat *self);

void Meerkat_sleep(Meerkat *self);

void Meerkat_show(Meerkat *self);
#endif