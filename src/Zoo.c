#include "./include/Zoo.h"

void create_zoo(Zoo *zoo){
    zoo->time_elapsed = 0;
    
    Vector v1 = vec_new(OSTRICHS, sizeof(ProducerConsumerBuffer), LOOSE);
    memmove(&zoo->ostrichs_com, &v1, sizeof(Vector));

    Vector v2 = vec_new(MEERKATS, sizeof(ProducerConsumerBuffer), LOOSE);
    memmove(&zoo->meerkats_com, &v2, sizeof(Vector));
    
    Vector v3 = vec_new(LIONS, sizeof(ProducerConsumerBuffer), LOOSE);
    memmove(&zoo->lions_com, &v3, sizeof(Vector));

    Vector v4 = vec_new(VETERINARIANS, sizeof(Veterinarian), LOOSE);
    memmove(&zoo->veterinarians, &v4, sizeof(Vector));

    Vector v5 = vec_new(OSTRICHS, sizeof(Ostrich), LOOSE);
    memmove(&zoo->ostrichs, &v5, sizeof(Vector));
    
    Vector v6 = vec_new(LION, sizeof(Lion), LOOSE);
    memmove(&zoo->lions, &v6, sizeof(Vector));
    
    Vector v7 = vec_new(MEERKAT, sizeof(Meerkat), LOOSE);
    memmove(&zoo->meerkats, &v7, sizeof(Vector));

    Vector v8 = vec_new(0, sizeof(pthread_t), LOOSE);
    memmove(&zoo->threads, &v8, sizeof(Vector));

    instantiate_actors(zoo);
    semaphore_init(zoo);
    spawn_threads(zoo);
}

void spawn_threads(Zoo *zoo){
    // Launch Ostrichs threads
    for (size_t i = 0; i < OSTRICHS; ++i)
    {
        OstrichArgs *ostrich_args = malloc(sizeof(OstrichArgs));
        assert(ostrich_args != NULL);
        ostrich_args->feeder_ammount = &zoo->ostrich_food_filled;
        ostrich_args->feeder_ammount_spaces_empty = &zoo->ostrich_food_slots_available;
        ostrich_args->ostrich_instance = vec_get_addr_pos(&zoo->ostrichs, i);
        ostrich_args->incoming_communication = vec_get_addr_pos(&zoo->ostrichs_com, i);
        ostrich_args->veterinarian_comunication = &zoo->veterinarians_com;

        // add_consumer_to_buffer(ostrich_args->incoming_communication);
        add_producer_to_buffer(ostrich_args->veterinarian_comunication);

        pthread_t tid;
        pthread_create(&tid, NULL, Ostrich_run_routine, (void *) ostrich_args);
        vec_push(&zoo->threads, &tid);
    }

    // Launch Lion threads
    for (size_t i = 0; i < LIONS; ++i)
    {
        LionArgs *lion_args = malloc(sizeof(LionArgs));
        assert(lion_args != NULL);
        lion_args->feeder_ammount = &zoo->lion_food_filled;
        lion_args->feeder_ammount_spaces_empty = &zoo->lion_food_slots_available;
        lion_args->lion_instance = vec_get_addr_pos(&zoo->lions, i);
        lion_args->incoming_communication = vec_get_addr_pos(&zoo->lions_com, i);
        lion_args->veterinarian_comunication = &zoo->veterinarians_com;

        // add_consumer_to_buffer(lion_args->incoming_communication);
        add_producer_to_buffer(lion_args->veterinarian_comunication);

        pthread_t tid;
        pthread_create(&tid, NULL, Lion_run_routine, (void *) lion_args);
        vec_push(&zoo->threads, &tid);
    }

    // Launch Meerkat threads
    for (size_t i = 0; i < MEERKATS; ++i)
    {
        MeerkatArgs *meerkat_args = malloc(sizeof(MeerkatArgs));
        assert(meerkat_args != NULL);
        meerkat_args->feeder_ammount = &zoo->meerkat_food_filled;
        meerkat_args->feeder_ammount_spaces_empty = &zoo->meerkat_food_slots_available;
        meerkat_args->meerkat_instance = vec_get_addr_pos(&zoo->meerkats, i);
        meerkat_args->incoming_communication = vec_get_addr_pos(&zoo->meerkats_com, i);
        meerkat_args->veterinarian_comunication = &zoo->veterinarians_com;

        // add_consumer_to_buffer(meerkat_args->incoming_communication);
        add_producer_to_buffer(meerkat_args->veterinarian_comunication);

        pthread_t tid;
        pthread_create(&tid, NULL, Meerkat_run_routine, (void *) meerkat_args);
        vec_push(&zoo->threads, &tid);
    }

    // Launch Veterinarians threads
    for (size_t i = 0; i < VETERINARIANS; ++i)
    {
        VeterinarianArgs *veterinarian_args = malloc(sizeof(VeterinarianArgs));
        assert(veterinarian_args != NULL);

        veterinarian_args->incoming_communication = &zoo->veterinarians_com;
        veterinarian_args->vet_instance = vec_get_addr_pos(&zoo->veterinarians, i);
        veterinarian_args->supplier_communication = &zoo->supplier_com;
        
        veterinarian_args->lion_food_filled = &zoo->lion_food_filled;
        veterinarian_args->lion_food_slots_available = &zoo->lion_food_slots_available;
        veterinarian_args->lion_food_storage = &zoo->lion_food_storage;
        
        veterinarian_args->ostrich_food_filled = &zoo->ostrich_food_filled;
        veterinarian_args->ostrich_food_slots_available = &zoo->ostrich_food_slots_available;
        veterinarian_args->ostrich_food_storage = &zoo->ostrich_food_storage;

        veterinarian_args->meerkat_food_filled = &zoo->meerkat_food_filled;
        veterinarian_args->meerkat_food_slots_available = &zoo->meerkat_food_slots_available;
        veterinarian_args->meerkat_food_storage = &zoo->meerkat_food_storage;

        // add_consumer_to_buffer(veterinarian_args->incoming_communication);
        add_producer_to_buffer(veterinarian_args->supplier_communication);

        pthread_t tid;
        pthread_create(&tid, NULL, Veterinarian_run_routine, (void *) veterinarian_args);
        vec_push(&zoo->threads, &tid);
    }

    // Launch Supplier Thread

    SupplierArgs *supplier_args = malloc(sizeof(SupplierArgs));
    assert(supplier_args != NULL);

    supplier_args->supplier_object = &zoo->supplier;
    supplier_args->lion_food_storage = &zoo->lion_food_storage;
    supplier_args->meerkat_food_storage = &zoo->meerkat_food_storage;
    supplier_args->ostrich_food_storage = &zoo->ostrich_food_storage;
    supplier_args->incoming_communication = &zoo->supplier_com;

    pthread_t tid;
    pthread_create(&tid, NULL, Supplier_run_routine, (void *)supplier_args);
    vec_push(&zoo->threads, &tid);
}

void semaphore_init(Zoo *zoo){

    sem_init(&zoo->meerkat_food_storage, 0, MAX_STOCKED_FOOD);
    sem_init(&zoo->ostrich_food_storage, 0, MAX_STOCKED_FOOD);
    sem_init(&zoo->lion_food_storage, 0, MAX_STOCKED_FOOD);

    sem_init(&zoo->meerkat_food_filled, 0, MAX_AVAILABLE_FOOD);
    sem_init(&zoo->ostrich_food_filled, 0, MAX_AVAILABLE_FOOD);
    sem_init(&zoo->lion_food_filled, 0, MAX_AVAILABLE_FOOD);

    sem_init(&zoo->meerkat_food_slots_available, 0, 0);
    sem_init(&zoo->ostrich_food_slots_available, 0, 0);
    sem_init(&zoo->lion_food_slots_available, 0, 0);
}

void instantiate_actors(Zoo *zoo){
    
    // Instanciate every actor and add to its respective vector
    for (size_t i = 0; i < OSTRICHS; i++)
    {
        Ostrich ostrich = {
            .animal = {
                .id = i+1,
                .eaten = 0,
                .time_left_to_sleep = 0,
                .state = EAT_1,
            }
        };
        vec_push(&zoo->ostrichs, &ostrich);

        ProducerConsumerBuffer buf = create_producer_consumer_buffer(MAXIMUM_MESSAGES, sizeof(Message));
        add_producer_to_buffer(&buf);
        add_consumer_to_buffer(&buf);
        vec_push(&zoo->ostrichs_com, &buf);
    }

    for (size_t i = 0; i < MEERKATS; i++)
    {
        Meerkat meerkat = {
            .animal = {
                .id = i+1,
                .eaten = 0,
                .time_left_to_sleep = 0,
                .state = EAT_1,
            }
        };
        vec_push(&zoo->meerkats, &meerkat);

        ProducerConsumerBuffer buf = create_producer_consumer_buffer(MAXIMUM_MESSAGES, sizeof(Message));
        add_producer_to_buffer(&buf);
        add_consumer_to_buffer(&buf);
        vec_push(&zoo->meerkats_com, &buf);
    }

    for (size_t i = 0; i < LIONS; i++)
    {
        Lion lion = {
            .animal = {
                .id = i+1,
                .eaten = 0,
                .time_left_to_sleep = 0,
                .state = EAT_1,
            }
        };
        vec_push(&zoo->lions, &lion);

        ProducerConsumerBuffer buf = create_producer_consumer_buffer(MAXIMUM_MESSAGES, sizeof(Message));
        add_producer_to_buffer(&buf);
        add_consumer_to_buffer(&buf);
        vec_push(&zoo->lions_com, &buf);
    }

    for (size_t i = 0; i < VETERINARIANS; i++)
    {
        Veterinarian veterinarian = {
            .id = i+1,
        };
        vec_push(&zoo->veterinarians, &veterinarian);  
    }
    
    ProducerConsumerBuffer vets_com = create_producer_consumer_buffer(MAXIMUM_MESSAGES, sizeof(Message)); 
    add_producer_to_buffer(&vets_com);
    add_consumer_to_buffer(&vets_com);
    memmove(&zoo->veterinarians_com, &vets_com, sizeof(ProducerConsumerBuffer));

    ProducerConsumerBuffer suppliers_com = create_producer_consumer_buffer(MAXIMUM_MESSAGES, sizeof(Message));
    add_producer_to_buffer(&suppliers_com);
    add_consumer_to_buffer(&suppliers_com);
    memmove(&zoo->supplier_com, &suppliers_com, sizeof(ProducerConsumerBuffer));

    // Instantiate Supplier

    Supplier supplier = {
        .id = 1,
        .lion_food_supplied = 0,
        .meerkat_food_supplied = 0,
        .ostrich_food_supplied = 0,
    };

    zoo->supplier = supplier;
}

void print_zoo_stats(Zoo *zoo){
    pthread_mutex_lock(&mutex_stdout);
    
    printf("Time elapsed: %u\n\n", zoo->time_elapsed);
    
    Ostrich ostrich;
    for (uint32_t c = 0; c < zoo->ostrichs.len; ++c){
        vec_get(&zoo->ostrichs, c, &ostrich);
        printf("Ostrich id: %d, eaten: %d, state: %d \n", ostrich.animal.id, ostrich.animal.eaten, ostrich.animal.state);
    }

    printf("\n");

    Lion lion;
    for (uint32_t c = 0; c < zoo->lions.len; ++c){
        vec_get(&zoo->lions, c, &lion);
        printf("Lion id: %d, eaten: %d, state: %d \n", lion.animal.id, lion.animal.eaten, lion.animal.state);
    }

    printf("\n");

    Meerkat meerkat;
    for (uint32_t c = 0; c < zoo->meerkats.len; ++c){
        vec_get(&zoo->meerkats, c, &meerkat);
        printf("Meerkat id: %d, eaten: %d, state: %d \n", meerkat.animal.id, meerkat.animal.eaten, meerkat.animal.state);
    }

    printf("\n");

    pthread_mutex_unlock(&mutex_stdout);
}

void free_zoo(Zoo *zoo){
    vec_free(&zoo->ostrichs);
    vec_free(&zoo->lions);
    vec_free(&zoo->meerkats);

    ProducerConsumerBuffer buf;
    while (!vec_is_empty(&zoo->ostrichs_com)){
        vec_pop(&zoo->ostrichs_com, &buf);
        while (has_producers_buffer(buf)) {remove_producer_from_buffer(&buf);}
        free_ProducerConsumerBuffer(&buf);
    }
    vec_free(&zoo->ostrichs_com);
    
    while (!vec_is_empty(&zoo->lions_com)){
        vec_pop(&zoo->lions_com, &buf);
        while (has_producers_buffer(buf)) {remove_producer_from_buffer(&buf);}
        free_ProducerConsumerBuffer(&buf);
    }
    vec_free(&zoo->lions_com);
    
    while (!vec_is_empty(&zoo->meerkats_com)){
        vec_pop(&zoo->meerkats_com, &buf);
        while (has_producers_buffer(buf)) {remove_producer_from_buffer(&buf);}
        free_ProducerConsumerBuffer(&buf);
    } 
    vec_free(&zoo->meerkats_com);

    free_ProducerConsumerBuffer(&zoo->supplier_com);
    free_ProducerConsumerBuffer(&zoo->veterinarians_com);

    sem_destroy(&zoo->meerkat_food_storage);
    sem_destroy(&zoo->lion_food_storage);
    sem_destroy(&zoo->ostrich_food_storage);

    sem_destroy(&zoo->meerkat_food_filled);
    sem_destroy(&zoo->lion_food_filled);
    sem_destroy(&zoo->ostrich_food_filled);

    sem_destroy(&zoo->meerkat_food_slots_available);
    sem_destroy(&zoo->ostrich_food_slots_available);
    sem_destroy(&zoo->lion_food_slots_available);

    vec_free(&zoo->ostrichs);
    vec_free(&zoo->lions);
    vec_free(&zoo->meerkats);
    vec_free(&zoo->veterinarians);

    vec_free(&zoo->threads);
}

void increment_one_hour(Zoo *zoo){
    pthread_mutex_lock(&mutex_stdout);
    printf("\tHour %d\n", zoo->time_elapsed);
    pthread_mutex_unlock(&mutex_stdout);

    Message m = {
        .message.sys_message = CLOCK,
        .type = SYSTEM_MESSAGE,
    };
    
    // Send a clock to all lions
    for (size_t c = 0; c < zoo->lions_com.len; ++c){
        insert_to_buffer(vec_get_addr_pos(&zoo->lions_com, c), &m);
    }

    // Send a clock to all ostrichs
    for (size_t c = 0; c < zoo->ostrichs_com.len; ++c){
        insert_to_buffer(vec_get_addr_pos(&zoo->ostrichs_com, c), &m);
    }

    // Send a clock to all meerkats
    for (size_t c = 0; c < zoo->meerkats_com.len; ++c){
        insert_to_buffer(vec_get_addr_pos(&zoo->meerkats_com, c), &m);
    }

    ++zoo->time_elapsed;
}

void terminate_all_threads(Zoo *zoo) {
    // Send a message requesting all animals, veterinarians and supplier threads to terminate
    
    Message m = {
        .message.sys_message = TERMINATE,
        .type = SYSTEM_MESSAGE,
    };
    
    for (size_t c = 0; c < zoo->lions_com.len; ++c){
        insert_to_buffer(vec_get_addr_pos(&zoo->lions_com, c), &m);
    }

    for (size_t c = 0; c < zoo->ostrichs_com.len; ++c){
        insert_to_buffer(vec_get_addr_pos(&zoo->ostrichs_com, c), &m);
    }

    for (size_t c = 0; c < zoo->meerkats_com.len; ++c){
        insert_to_buffer(vec_get_addr_pos(&zoo->meerkats_com, c), &m);
    }

    for (size_t c = 0; c < zoo->veterinarians.len; ++c) {
        insert_to_buffer(&zoo->veterinarians_com, &m);
    }

    insert_to_buffer(&zoo->supplier_com, &m);

    // Join all spawned threads
    pthread_t t_id;
    for (size_t c = 0; c < zoo->threads.len; ++c) {
        vec_get(&zoo->threads, c, &t_id);
        pthread_join(t_id, NULL);
    }

}

void *zoo_run_routine(void *args){   
    Zoo *zoo = (Zoo *) args;

    create_zoo(zoo);

    const uint32_t hours_total = 3 * 24; // Three days
    uint32_t hours_simulated = 0;
    
    while (hours_simulated < hours_total) {
        increment_one_hour(zoo);
        ++hours_simulated;
        sleep(1);
    }

    terminate_all_threads(zoo);

    print_zoo_stats(zoo);

    return NULL;
}