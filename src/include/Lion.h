#ifndef LION_H
#define LION_H

#include <unistd.h>
#include <pthread.h>
#include "ProducerConsumer.h"
#include "Animal.h"

extern pthread_mutex_t mutex_stdout;

#define LION_MAX_SLEEP_TIME 12
#define LION_MIN_SLEEP_TIME 8
#define LION_MIN_EAT 2
#define LION_MAX_EAT 5

typedef struct {
    Animal animal;
} Lion;

void *Lion_run_routine(void *args);

void Lion_eat(Lion *self);

void Lion_sleep(Lion *self);

void Lion_show(Lion *self);
#endif
