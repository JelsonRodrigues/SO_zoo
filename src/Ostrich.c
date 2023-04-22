#include "./include/Ostrich.h"

void *Ostrich_run_routine(void *args){
    OstrichArgs *args_local = (OstrichArgs *) args;

    Message incoming_message;
    bool received = false;
    do {
        received = get_item_from_buffer(args_local->incoming_communication, &incoming_message);

        if (received){
            switch (incoming_message.type)
            {
            case SYSTEM_MESSAGE:
                switch (incoming_message.message.sys_message)
                {
                case CLOCK:
                    Ostrich_handle_clock(args);
                    break;
                case TERMINATE:
                    Ostrich_print_log(args_local, "terminating...");
                    remove_consumer_from_buffer(args_local->incoming_communication);
                    remove_producer_from_buffer(args_local->veterinarian_comunication);
                    free(args);
                    return NULL;
                    break;
                default:
                    break;
                }
                break;
            default:
            char *string;
            asprintf(&string, "Unknown message type %d!!", incoming_message.type);
            Ostrich_print_log(args_local, string);
            free(string);
            break;
            }
        }
    } while (received);
    
    remove_consumer_from_buffer(args_local->incoming_communication);
    remove_producer_from_buffer(args_local->veterinarian_comunication);
    free(args);
    return NULL;
}

void Ostrich_handle_clock(OstrichArgs *args){
    switch (args->ostrich_instance->animal.state)
    {
    case EAT_1:
        Ostrich_eat(args);
        break;
    case EAT_2:
        Ostrich_eat(args);
        break;
    case SLEEP:
        Ostrich_sleep(args);
        break;
    case SHOW:
        Ostrich_show(args);
        break;
    default:       
        char *string;
        asprintf(&string, "Unknown state %d, Reseting to state SLEEP!!", args->ostrich_instance->animal.state);
        Ostrich_print_log(args, string);
        free(string);

        args->ostrich_instance->animal.time_left_to_sleep = OSTRICH_MAX_SLEEP_TIME;
        Ostrich_sleep(args);
        break;
    }
}

void Ostrich_eat(OstrichArgs *args){
    const uint32_t ammount_to_eat = rand() % (OSTRICH_MAX_EAT - OSTRICH_MIN_EAT) + OSTRICH_MIN_EAT;

    // Warn veterinarians that it will eat
    Message m = {
        .message.animal_eat = {
            .animal = OSTRICH, 
            .ammount = ammount_to_eat
        },
        .type = ANIMAL_EAT,
    };
    insert_to_buffer(args->veterinarian_comunication, &m);
    
    char *string;
    asprintf(&string, "goint to EAT %u ammount", ammount_to_eat);
    Ostrich_print_log(args, string);
    free(string);   

    uint32_t eaten = 0;
    
    while (eaten < ammount_to_eat) {
        sem_wait(args->feeder_ammount);
        sem_post(args->feeder_ammount_spaces_empty);
        ++eaten;
        ++args->ostrich_instance->animal.eaten;
    }

    Ostrich_print_log(args, "finished eating");

    Ostrich_change_state(args);
}

void Ostrich_sleep(OstrichArgs *args){
    if (args->ostrich_instance->animal.time_left_to_sleep > 0) {
        char *string;
        asprintf(&string, "sleeping for more %u hours", args->ostrich_instance->animal.time_left_to_sleep);
        Ostrich_print_log(args, string);
        free(string);

        --args->ostrich_instance->animal.time_left_to_sleep;
    }
    else {
        Ostrich_change_state(args);
    }
}

void Ostrich_show(OstrichArgs *args){
    Ostrich_print_log(args, "AAAAHN! Showing itself");
    Ostrich_change_state(args);
}


void Ostrich_change_state(OstrichArgs *args) {
    args->ostrich_instance->animal.state = (args->ostrich_instance->animal.state + 1) % NUMBER_OF_STATES;
    if (args->ostrich_instance->animal.state == SLEEP) {
        args->ostrich_instance->animal.time_left_to_sleep = rand() % (OSTRICH_MAX_SLEEP_TIME - OSTRICH_MIN_SLEEP_TIME) + OSTRICH_MIN_SLEEP_TIME;
    }
}

void Ostrich_print_log(OstrichArgs *args, char *message) {
    pthread_mutex_lock(&mutex_stdout);
    printf("OSTRICH ID %u: %s\n", args->ostrich_instance->animal.id, message);
    pthread_mutex_unlock(&mutex_stdout);
}