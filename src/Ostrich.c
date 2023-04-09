#include "./include/Ostrich.h"

void *Ostrich_run_routine(void *args){
    OstrichArgs *args_local = (OstrichArgs *) args;

    Message incomming_message;
    bool received = false;
    do {
        received = get_item_from_buffer(args_local->incomming_communication, &incomming_message);

        if (received){
            switch (incomming_message)
            {
            case CLOCK:
                Ostrich_handle_clock(args_local);
                break;
            case TERMINATE:
                break;
            default:
                pthread_mutex_lock(&mutex_stdout);
                printf("Unknown MESSAGE %d received by Ostrich id %d\n", incomming_message, args_local->ostrich_instance->animal.id);
                pthread_mutex_unlock(&mutex_stdout);
                break;
            }
        }
    } while (received);
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
    
    pthread_mutex_lock(&mutex_stdout);
    printf("OSTRICH id %u going to EAT %u ammount\n", args->ostrich_instance->animal.id, ammount_to_eat);
    pthread_mutex_unlock(&mutex_stdout);     

    // Warn veterinarians that it will eat
    sem_post(args->animals_waiting_or_eating);
    for (int c = 0; c < ammount_to_eat; ++c){
        sem_post(args->ostrich_food_needed);
    }

    int32_t eaten = 0;
    
    while (eaten < ammount_to_eat) {
        ++eaten;
        sem_wait(args->feeder_ammount);
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