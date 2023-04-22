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
                    Veterinarian_print_log(local_args, "terminating...");
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
                char *string;
                asprintf(&string, "Unknown type of message received %d", incoming_message.type);
                Veterinarian_print_log(local_args, string);
                free(string);
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
        char *string;
        asprintf(&string, "Unknown type of animal to feed %d", message.animal);
        Veterinarian_print_log(args, string);
        free(string);
        break;
    }
}

void Veterinarian_fill_lion_eater(VeterinarianArgs *args, uint32_t ammount){
    uint32_t ammount_filled = 0;
    int32_t current_ammount = 0;
    
    sem_getvalue(args->lion_food_filled, &current_ammount);
    
    char *string;
    asprintf(&string, "Lion eater current ammount %d", current_ammount);
    Veterinarian_print_log(args, string);
    free(string);

    while (ammount_filled < ammount) {
        if (sem_trywait(args->lion_food_storage) == 0){
            sem_wait(args->lion_food_slots_available);
            sem_post(args->lion_food_filled);
            ++ammount_filled;
            ++args->vet_instance->total_feeded_lion;
        }
        else {
            Veterinarian_print_log(args, "Lion food storage of the Zoo is empty! Asking for a refill...");
            Veterinarian_request_stock_refill(args);
            sem_wait(args->lion_food_storage);
        }
    }
}

void Veterinarian_fill_ostrich_eater(VeterinarianArgs *args, uint32_t ammount){
    uint32_t ammount_filled = 0;
    int32_t current_ammount = 0;
    
    sem_getvalue(args->ostrich_food_filled, &current_ammount);

    char *string;
    asprintf(&string, "Ostrich eater current ammount %d", current_ammount);
    Veterinarian_print_log(args, string);
    free(string);

    while (ammount_filled < ammount) {
        if (sem_trywait(args->ostrich_food_storage) == 0){
            sem_wait(args->ostrich_food_slots_available);
            sem_post(args->ostrich_food_filled);
            ++ammount_filled;
            ++args->vet_instance->total_feeded_ostrich;
        }
        else {
            Veterinarian_print_log(args, "Ostrich food storage of the Zoo is empty! Asking for a refill...");
            Veterinarian_request_stock_refill(args);
            sem_wait(args->ostrich_food_storage);
        }
    }
}

void Veterinarian_fill_meerkat_eater(VeterinarianArgs *args, uint32_t ammount){
    uint32_t ammount_filled = 0;
    int32_t current_ammount = 0;
    
    sem_getvalue(args->meerkat_food_filled, &current_ammount);
    
    char *string;
    asprintf(&string, "Meerkat eater current ammount %d", current_ammount);
    Veterinarian_print_log(args, string);
    free(string);

    while (ammount_filled < ammount) {
        if (sem_trywait(args->meerkat_food_storage) == 0){
            sem_wait(args->meerkat_food_slots_available);
            sem_post(args->meerkat_food_filled);
            ++ammount_filled;
            ++args->vet_instance->total_feeded_meerkat;
        }
        else {
            Veterinarian_print_log(args, "Meerkat food storage of the Zoo is empty! Asking for a refill...");
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

    ++args->vet_instance->total_requests_for_food_reffil;
}

void Veterinarian_print_log(VeterinarianArgs *args, char *message) {
    pthread_mutex_lock(&mutex_stdout);
    printf("VETERINARIAN ID %u: %s\n", args->vet_instance->id, message);
    pthread_mutex_unlock(&mutex_stdout);
}