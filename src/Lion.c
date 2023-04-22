#include "./include/Lion.h"

void *Lion_run_routine(void *args){
    LionArgs *args_local = (LionArgs *) args;

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
                    Lion_handle_clock(args);
                    break;
                case TERMINATE:
                    Lion_print_log(args_local, "terminating...");
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
            Lion_print_log(args_local, string);
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

void Lion_handle_clock(LionArgs *args){
    switch (args->lion_instance->animal.state)
    {
    case EAT_1:
        Lion_eat(args);
        break;
    case EAT_2:
        Lion_eat(args);
        break;
    case SLEEP:
        Lion_sleep(args);
        break;
    case SHOW:
        Lion_show(args);
        break;
    default:
        char *string;
        asprintf(&string, "Unknown state %d, Reseting to state SLEEP!!", args->lion_instance->animal.state);
        Lion_print_log(args, string);
        free(string);
   
        args->lion_instance->animal.time_left_to_sleep = LION_MAX_SLEEP_TIME;
        Lion_sleep(args);
        break;
    }
}

void Lion_eat(LionArgs *args){
    const uint32_t ammount_to_eat = rand() % (LION_MAX_EAT - LION_MIN_EAT) + LION_MIN_EAT;

    // Warn veterinarians that it will eat
    Message m = {
        .message.animal_eat = {
            .animal = LION, 
            .ammount = ammount_to_eat
        },
        .type = ANIMAL_EAT,
    };
    insert_to_buffer(args->veterinarian_comunication, &m);

    char *string;
    asprintf(&string, "goint to EAT %u ammount", ammount_to_eat);
    Lion_print_log(args, string);
    free(string);

    uint32_t eaten = 0;
    
    while (eaten < ammount_to_eat) {
        ++eaten;
        sem_wait(args->feeder_ammount);
        sem_post(args->feeder_ammount_spaces_empty);
        ++args->lion_instance->animal.eaten;
    }

    Lion_print_log(args, "finished eating");

    Lion_change_state(args);
}

void Lion_sleep(LionArgs *args){
    if (args->lion_instance->animal.time_left_to_sleep > 0) {
        char *string;
        asprintf(&string, "sleeping for more %u hours", args->lion_instance->animal.time_left_to_sleep);
        Lion_print_log(args, string);
        free(string);

        --args->lion_instance->animal.time_left_to_sleep;
    }
    else {
        Lion_change_state(args);
    }
}

void Lion_show(LionArgs *args){
    Lion_print_log(args, "RAAAWR! Showing itself");
    Lion_change_state(args);
}

void Lion_change_state(LionArgs *args) {
    args->lion_instance->animal.state = (args->lion_instance->animal.state + 1) % NUMBER_OF_STATES;
    if (args->lion_instance->animal.state == SLEEP) {
        args->lion_instance->animal.time_left_to_sleep = rand() % (LION_MAX_SLEEP_TIME - LION_MIN_SLEEP_TIME) + LION_MIN_SLEEP_TIME;
    }
}

void Lion_print_log(LionArgs *args, char *message) {
    pthread_mutex_lock(&mutex_stdout);
    printf("LION ID %u: %s\n", args->lion_instance->animal.id, message);
    pthread_mutex_unlock(&mutex_stdout);
}