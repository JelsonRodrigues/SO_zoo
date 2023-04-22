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
            pthread_mutex_lock(&mutex_stdout);
            printf("Unknown MESSAGE %d received by Lion id %d\n", incoming_message.type, args_local->lion_instance->animal.id);
            pthread_mutex_unlock(&mutex_stdout);
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
        pthread_mutex_lock(&mutex_stdout);
        printf("Unknown state of Lion id %d, state %d\n Reseting to state SLEEP", args->lion_instance->animal.id, args->lion_instance->animal.state);
        pthread_mutex_unlock(&mutex_stdout);        
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
    
    pthread_mutex_lock(&mutex_stdout);
    printf("Lion id %u going to EAT %u ammount\n", args->lion_instance->animal.id, ammount_to_eat);
    pthread_mutex_unlock(&mutex_stdout);     

    uint32_t eaten = 0;
    
    while (eaten < ammount_to_eat) {
        ++eaten;
        sem_wait(args->feeder_ammount);
        sem_post(args->feeder_ammount_spaces_empty);
        ++args->lion_instance->animal.eaten;
    }

    pthread_mutex_lock(&mutex_stdout);
    printf("Lion id %u finished EAT %d\n", args->lion_instance->animal.id, eaten);
    pthread_mutex_unlock(&mutex_stdout);

    args->lion_instance->animal.state = (args->lion_instance->animal.state + 1) % NUMBER_OF_STATES;
}

void Lion_sleep(LionArgs *args){
    if (args->lion_instance->animal.time_left_to_sleep > 0) {
        pthread_mutex_lock(&mutex_stdout);
        printf("Lion ID: %u sleeping for more %u hours\n", args->lion_instance->animal.id, args->lion_instance->animal.time_left_to_sleep);
        pthread_mutex_unlock(&mutex_stdout);
        --args->lion_instance->animal.time_left_to_sleep;
    }
    else {
        args->lion_instance->animal.state = (args->lion_instance->animal.state + 1) % NUMBER_OF_STATES;
    }
}

void Lion_show(LionArgs *args){
    pthread_mutex_lock(&mutex_stdout);
    printf("RAAAWR! Lion ID: %u is showing itself\n", args->lion_instance->animal.id);
    pthread_mutex_unlock(&mutex_stdout);

    args->lion_instance->animal.state = (args->lion_instance->animal.state + 1) % NUMBER_OF_STATES;
}