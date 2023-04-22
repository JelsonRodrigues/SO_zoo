#include "./include/Meerkat.h"

void *Meerkat_run_routine(void *args){
    MeerkatArgs *args_local = (MeerkatArgs *) args;

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
                    Meerkat_handle_clock(args);
                    break;
                case TERMINATE:
                    Meerkat_print_log(args_local, "terminating...");
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
            Meerkat_print_log(args_local, string);
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

void Meerkat_handle_clock(MeerkatArgs *args){
    switch (args->meerkat_instance->animal.state)
    {
    case EAT_1:
        Meerkat_eat(args);
        break;
    case EAT_2:
        Meerkat_eat(args);
        break;
    case SLEEP:
        Meerkat_sleep(args);
        break;
    case SHOW:
        Meerkat_show(args);
        break;
    default:
        char *string;
        asprintf(&string, "Unknown state %d, Reseting to state SLEEP!!", args->meerkat_instance->animal.state);
        Meerkat_print_log(args, string);
        free(string);

        args->meerkat_instance->animal.time_left_to_sleep = MEERKAT_MAX_SLEEP_TIME;
        Meerkat_sleep(args);
        break;
    }
}

void Meerkat_eat(MeerkatArgs *args){
    const uint32_t ammount_to_eat = rand() % (MEERKAT_MAX_EAT - MEERKAT_MIN_EAT) + MEERKAT_MIN_EAT;

    // Warn veterinarians that it will eat
    Message m = {
        .message.animal_eat = {
            .animal = MEERKAT, 
            .ammount = ammount_to_eat
        },
        .type = ANIMAL_EAT,
    };
    insert_to_buffer(args->veterinarian_comunication, &m);
    
    char *string;
    asprintf(&string, "going to EAT %u ammount", ammount_to_eat);
    Meerkat_print_log(args, string);
    free(string);

    uint32_t eaten = 0;
    
    while (eaten < ammount_to_eat) {
        ++eaten;
        sem_wait(args->feeder_ammount);
        sem_post(args->feeder_ammount_spaces_empty);
        ++args->meerkat_instance->animal.eaten;
    }

    Meerkat_print_log(args, "finished eating");

    Meerkat_change_state(args);
}

void Meerkat_sleep(MeerkatArgs *args){
    if (args->meerkat_instance->animal.time_left_to_sleep > 0) {
        char *string;
        asprintf(&string, "sleeping for more %u hours", args->meerkat_instance->animal.time_left_to_sleep);
        Meerkat_print_log(args, string);
        free(string);
        --args->meerkat_instance->animal.time_left_to_sleep;
    }
    else {
        Meerkat_change_state(args);
    }
}

void Meerkat_show(MeerkatArgs *args){
    Meerkat_print_log(args, "GRRRRN! Showing itself");
    Meerkat_change_state(args);
}

void Meerkat_change_state(MeerkatArgs *args) {
    args->meerkat_instance->animal.state = (args->meerkat_instance->animal.state + 1) % NUMBER_OF_STATES;
    if (args->meerkat_instance->animal.state == SLEEP) {
        args->meerkat_instance->animal.time_left_to_sleep = rand() % (MEERKAT_MAX_SLEEP_TIME - MEERKAT_MIN_SLEEP_TIME) + MEERKAT_MIN_SLEEP_TIME;
    }
}

void Meerkat_print_log(MeerkatArgs *args, char *message) {
    pthread_mutex_lock(&mutex_stdout);
    printf("MEERKAT ID %u: %s\n", args->meerkat_instance->animal.id, message);
    pthread_mutex_unlock(&mutex_stdout);
}