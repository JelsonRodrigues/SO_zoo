#ifndef ANIMAL_H
#define ANIMAL_H

#include <stdint.h>

typedef enum {
    EAT_1, 
    SLEEP,
    EAT_2,
    SHOW,
    NUMBER_OF_STATES
} AnimalState;

typedef struct {
    uint32_t eaten;
    uint32_t id;
    uint32_t time_left_to_sleep;
    AnimalState state;
} Animal;

#endif