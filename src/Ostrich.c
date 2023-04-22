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
            printf("Unknown MESSAGE %d received by Ostrich id %d\n", incoming_message.type, args_local->ostrich_instance->animal.id);
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
        pthread_mutex_lock(&mutex_stdout);
        printf("Unknown state of Ostrich id %d, state %d\n Reseting to state SLEEP", args->ostrich_instance->animal.id, args->ostrich_instance->animal.state);
        pthread_mutex_unlock(&mutex_stdout);        
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
    
    pthread_mutex_lock(&mutex_stdout);
    printf("OSTRICH id %u going to EAT %u ammount\n", args->ostrich_instance->animal.id, ammount_to_eat);
    pthread_mutex_unlock(&mutex_stdout);     

    uint32_t eaten = 0;
    
    while (eaten < ammount_to_eat) {
        ++eaten;
        sem_wait(args->feeder_ammount);
        sem_post(args->feeder_ammount_spaces_empty);
        ++args->ostrich_instance->animal.eaten;
    }

    pthread_mutex_lock(&mutex_stdout);
    printf("OSTRICH id %u finished EAT %d\n", args->ostrich_instance->animal.id, eaten);
    pthread_mutex_unlock(&mutex_stdout);

    args->ostrich_instance->animal.state = (args->ostrich_instance->animal.state + 1) % NUMBER_OF_STATES;
}

void Ostrich_sleep(OstrichArgs *args){
    if (args->ostrich_instance->animal.time_left_to_sleep > 0) {
        pthread_mutex_lock(&mutex_stdout);
        printf("OSTRICH ID: %u sleeping for more %u hours\n", args->ostrich_instance->animal.id, args->ostrich_instance->animal.time_left_to_sleep);
        pthread_mutex_unlock(&mutex_stdout);
        --args->ostrich_instance->animal.time_left_to_sleep;
    }
    else {
        args->ostrich_instance->animal.state = (args->ostrich_instance->animal.state + 1) % NUMBER_OF_STATES;
    }
}

void Ostrich_show(OstrichArgs *args){
    pthread_mutex_lock(&mutex_stdout);
    printf("AAAAHN! OSTRICH ID: %u is showing itself\n", args->ostrich_instance->animal.id);
    pthread_mutex_unlock(&mutex_stdout);

    args->ostrich_instance->animal.state = (args->ostrich_instance->animal.state + 1) % NUMBER_OF_STATES;
}