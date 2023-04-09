#ifndef MESSAGE_H
#define MESSAGE_H

typedef enum {
    CLOCK,
    TERMINATE,
    LION_GOING_TO_EAT,
    MEERKET_GOING_TO_EAT,
    OSTRICH_GOING_TO_EAT,
    NEED_SUPPLY_REFILL,
} Message;

typedef enum {
    LIONS,
    MEERKATS,
    OSTRICHS,
    SUPPLIERS,
    VETERINARIANS,
} Actor;

#endif