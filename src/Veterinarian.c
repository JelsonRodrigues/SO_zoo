#include "./include/Veterinarian.h"

void *Veterinarian_run_routine(void *args){
    VeterinarianArgs *local_args = (VeterinarianArgs *) args;
    
    bool received = false;
    Message incoming_message;
    do {
        received = get_item_from_buffer(local_args->incoming_communication, &incoming_message);

        if (received) {
            switch (incoming_message.type)
            {
            case SYSTEM_MESSAGE:
                if (incoming_message.message.sys_message == TERMINATE) {
                    pthread_mutex_lock(&mutex_stdout);
                    printf("Veterinarian id %d terminating\n", local_args->vet_instance->id);
                    pthread_mutex_unlock(&mutex_stdout);
                    
                    remove_consumer_from_buffer(local_args->incoming_communication);
                    remove_producer_from_buffer(local_args->supplier_communication);
                    free(args);
                    return NULL;
                }
                break;
            case ANIMAL_EAT:
                Veterinarian_handle_animal_eat_message(local_args, incoming_message.message.animal_eat);
                break;
            default:
                pthread_mutex_lock(&mutex_stdout);
                printf("Unknown type of message received by veterinarian id %d, message %d\n", local_args->vet_instance->id, incoming_message.type);
                pthread_mutex_unlock(&mutex_stdout);
                break;
            }
        }
    } while (received);
    
    remove_consumer_from_buffer(local_args->incoming_communication);
    remove_producer_from_buffer(local_args->supplier_communication);
    free(args);
    return NULL;
}

void Veterinarian_handle_animal_eat_message(VeterinarianArgs *args, AnimalEat message) {
    switch (message.animal)
    {
    case LION:
        Veterinarian_fill_lion_eater(args, message.ammount);
        break;
    case OSTRICH:
        Veterinarian_fill_ostrich_eater(args, message.ammount);
        break;
    case MEERKAT:
        Veterinarian_fill_meerkat_eater(args, message.ammount);
        break;
    default:
        pthread_mutex_lock(&mutex_stdout);
        printf("Unknown type of animal to feed %d, received by veterinarian %d\n", message.animal, args->vet_instance->id);
        pthread_mutex_unlock(&mutex_stdout);
        break;
    }
}

void Veterinarian_fill_lion_eater(VeterinarianArgs *args, uint32_t ammount){
    uint32_t ammount_filled = 0;
    int32_t current_ammount = 0;
    
    sem_getvalue(args->lion_food_filled, &current_ammount);
    
    pthread_mutex_lock(&mutex_stdout);
    printf("Lion eater current supply %d\n", current_ammount);
    pthread_mutex_unlock(&mutex_stdout);

    while (ammount_filled < ammount) {
        if (sem_trywait(args->lion_food_storage) == 0){
            sem_wait(args->lion_food_slots_available);
            sem_post(args->lion_food_filled);
            ++ammount_filled;
        }
        else {
            pthread_mutex_lock(&mutex_stdout);
            printf("Lion food storage of the Zoo is empty! Asking for a refill...");
            pthread_mutex_unlock(&mutex_stdout);
            Veterinarian_request_stock_refill(args);
            sem_wait(args->lion_food_storage);
        }
    }
}

void Veterinarian_fill_ostrich_eater(VeterinarianArgs *args, uint32_t ammount){
    uint32_t ammount_filled = 0;
    
    int32_t current_ammount = 0;
    sem_getvalue(args->ostrich_food_slots_available, &current_ammount);
    
    pthread_mutex_lock(&mutex_stdout);
    printf("Ostrich eater current supply %d\n", current_ammount);
    pthread_mutex_unlock(&mutex_stdout);

    while (ammount_filled < ammount) {
        if (sem_trywait(args->ostrich_food_storage) == 0){
            sem_post(args->ostrich_food_slots_available);
            ++ammount_filled;
        }
        else {
            pthread_mutex_lock(&mutex_stdout);
            printf("Ostrich food storage of the Zoo is empty! Asking for a refill...");
            pthread_mutex_unlock(&mutex_stdout);
            Veterinarian_request_stock_refill(args);
            sem_wait(args->ostrich_food_storage);
        }
    }
}

void Veterinarian_fill_meerkat_eater(VeterinarianArgs *args, uint32_t ammount){
    uint32_t ammount_filled = 0;
    
    int32_t current_ammount = 0;
    sem_getvalue(args->meerkat_food_slots_available, &current_ammount);
    
    pthread_mutex_lock(&mutex_stdout);
    printf("Meerkat eater current supply %d\n", current_ammount);
    pthread_mutex_unlock(&mutex_stdout);

    while (ammount_filled < ammount) {
        if (sem_trywait(args->meerkat_food_storage) == 0){
            sem_post(args->meerkat_food_slots_available);
            ++ammount_filled;
        }
        else {
            pthread_mutex_lock(&mutex_stdout);
            printf("Meerkat food storage of the Zoo is empty! Asking for a refill...");
            pthread_mutex_unlock(&mutex_stdout);
            Veterinarian_request_stock_refill(args);
            sem_wait(args->meerkat_food_storage);
        }
    }
}

void Veterinarian_request_stock_refill(VeterinarianArgs *args){
    Message m = {
        .message = {},
        .type = REQUEST_SUPPLY_REFILL
    };

    insert_to_buffer(args->supplier_communication, &m);
}