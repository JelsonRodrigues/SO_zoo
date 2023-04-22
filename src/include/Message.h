#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

typedef enum {
    SYSTEM_MESSAGE,
    ANIMAL_EAT,
    REQUEST_SUPPLY_REFILL,
} MessageType;

typedef enum {
    CLOCK,
    TERMINATE,
} SystemMessage;

typedef enum {
    LION,
    MEERKAT,
    OSTRICH,
} AnimalType;

typedef struct {
    AnimalType animal;
    uint32_t ammount;
} AnimalEat;

typedef struct {} SupplyRefill;

typedef struct {
    union {
        AnimalEat animal_eat;
        SystemMessage sys_message;
        SupplyRefill sup_refill;
    } message;
    MessageType type;
} Message;

#endif