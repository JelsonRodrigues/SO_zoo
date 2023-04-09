#include "./include/Veterinarian.h"

void *Veterinarian_run_routine(void *args){
    VeterinarianArgs *local_args = (VeterinarianArgs *) args;
   // Message incomming_message;
    // do {
    //     received = get_item_from_buffer(local_args->incoming_communication, &incomming_message);

    //     if (received) {
    //         switch (incomming_message)
    //         {
    //         case LION_GOING_TO_EAT:
    //             Veterinarian_fill_lion_eater(local_args);
    //             break;
    //         case OSTRICH_GOING_TO_EAT:
    //             Veterinarian_fill_ostrich_eater(local_args);
    //             break;
    //         case MEERKET_GOING_TO_EAT:
    //             Veterinarian_fill_meerkat_eater(local_args);
    //             break;
    //         default:
    //             pthread_mutex_lock(&mutex_stdout);
    //             printf("Veterinarian ID %u received unknown message %d\n", local_args->vet_instance->id, incomming_message);
    //             pthread_mutex_unlock(&mutex_stdout);
    //             break;
    //         }
    //     }
    // } while (received);
    
    do {
        
        
    } while (true);


    return NULL;
}

void Veterinarian_fill_lion_eater(VeterinarianArgs *args){
    int current_ammount = 0;
    sem_getvalue(args->lion_food_available, &current_ammount);
    
    pthread_mutex_lock(&mutex_stdout);
    printf("Lion eater current supply %d\n", current_ammount);
    pthread_mutex_unlock(&mutex_stdout);

    while (current_ammount < MAX_AVAILABLE_FOOD) {
        if (sem_trywait(args->lion_food_storage) == 0){
            sem_post(args->lion_food_available);
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

void Veterinarian_fill_ostrich_eater(VeterinarianArgs *args){
    int current_ammount = 0;
    sem_getvalue(args->ostrich_food_available, &current_ammount);

    pthread_mutex_lock(&mutex_stdout);
    printf("Ostrich eater current supply %d\n", current_ammount);
    pthread_mutex_unlock(&mutex_stdout);

    while (current_ammount < MAX_AVAILABLE_FOOD) {
        if (sem_trywait(args->ostrich_food_storage) == 0){
            sem_post(args->ostrich_food_available);
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

void Veterinarian_fill_meerkat_eater(VeterinarianArgs *args){
    int current_ammount = 0;
    sem_getvalue(args->meerkat_food_available, &current_ammount);

    pthread_mutex_lock(&mutex_stdout);
    printf("Meerkat eater current supply %d\n", current_ammount);
    pthread_mutex_unlock(&mutex_stdout);

    while (current_ammount < MAX_AVAILABLE_FOOD) {
        if (sem_trywait(args->meerkat_food_storage) == 0){
            sem_post(args->meerkat_food_available);
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
    Message m = NEED_SUPPLY_REFILL;
    insert_to_buffer(args->supplier_communication, &m);
}