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
            printf("Unknown MESSAGE %d received by Meerkat id %d\n", incoming_message.type, args_local->meerkat_instance->animal.id);
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
        pthread_mutex_lock(&mutex_stdout);
        printf("Unknown state of Meerkat id %d, state %d\n Reseting to state SLEEP", args->meerkat_instance->animal.id, args->meerkat_instance->animal.state);
        pthread_mutex_unlock(&mutex_stdout);        
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
    
    pthread_mutex_lock(&mutex_stdout);
    printf("Meerkat id %u going to EAT %u ammount\n", args->meerkat_instance->animal.id, ammount_to_eat);
    pthread_mutex_unlock(&mutex_stdout);     

    uint32_t eaten = 0;
    
    while (eaten < ammount_to_eat) {
        ++eaten;
        sem_wait(args->feeder_ammount);
        sem_post(args->feeder_ammount_spaces_empty);
        ++args->meerkat_instance->animal.eaten;
    }

    pthread_mutex_lock(&mutex_stdout);
    printf("Meerkat id %u finished EAT %d\n", args->meerkat_instance->animal.id, eaten);
    pthread_mutex_unlock(&mutex_stdout);

    args->meerkat_instance->animal.state = (args->meerkat_instance->animal.state + 1) % NUMBER_OF_STATES;
}

void Meerkat_sleep(MeerkatArgs *args){
    if (args->meerkat_instance->animal.time_left_to_sleep > 0) {
        pthread_mutex_lock(&mutex_stdout);
        printf("Meerkat ID: %u sleeping for more %u hours\n", args->meerkat_instance->animal.id, args->meerkat_instance->animal.time_left_to_sleep);
        pthread_mutex_unlock(&mutex_stdout);
        --args->meerkat_instance->animal.time_left_to_sleep;
    }
    else {
        args->meerkat_instance->animal.state = (args->meerkat_instance->animal.state + 1) % NUMBER_OF_STATES;
    }
}

void Meerkat_show(MeerkatArgs *args){
    pthread_mutex_lock(&mutex_stdout);
    printf("GRRRRN! Meerkat ID: %u is showing itself\n", args->meerkat_instance->animal.id);
    pthread_mutex_unlock(&mutex_stdout);

    args->meerkat_instance->animal.state = (args->meerkat_instance->animal.state + 1) % NUMBER_OF_STATES;
}