#include "./include/Supplier.h"

void *Supplier_run_routine(void *args){
    SupplierArgs *local_args = (SupplierArgs *) args;

    bool received = false;
    Message incomming_message;
    do {
        received = get_item_from_buffer(local_args->incoming_communication, &incomming_message);

        if (received) {
            switch (incomming_message.type)
            {
            case REQUEST_SUPPLY_REFILL:
                pthread_mutex_lock(&mutex_stdout);
                printf("Supplier got a refill request\n");
                pthread_mutex_unlock(&mutex_stdout);
                supply(local_args);
                break;
            case SYSTEM_MESSAGE :
                if (incomming_message.message.sys_message == TERMINATE){
                    remove_consumer_from_buffer(local_args->incoming_communication);
                    free(args);
                    return NULL;
                }
                break;
            default:
                pthread_mutex_lock(&mutex_stdout);
                printf("Supplier received unknown message %d\n", incomming_message.type);
                pthread_mutex_unlock(&mutex_stdout);
                break;
            }
        }
    } while (received);

    remove_consumer_from_buffer(local_args->incoming_communication);
    free(args);
    return NULL;
}

void supply(SupplierArgs *args) {
    uint32_t lion_food_supplied = 0;
    uint32_t meerkat_food_supplied = 0;
    uint32_t ostrich_food_supplied = 0;

    int32_t lion_food_before = 0;
    int32_t meerkat_food_before = 0;
    int32_t ostrich_food_before = 0; 

    sem_getvalue(args->lion_food_storage, &lion_food_before);

    while (lion_food_before + lion_food_supplied < MAX_STOCKED_FOOD) {
        sem_post(args->lion_food_storage);
        ++lion_food_supplied;
    }

    sem_getvalue(args->meerkat_food_storage, &meerkat_food_before);

    while (meerkat_food_before + meerkat_food_supplied < MAX_STOCKED_FOOD) {
        sem_post(args->meerkat_food_storage);
        ++meerkat_food_supplied;
    }

    sem_getvalue(args->ostrich_food_storage, &ostrich_food_before);

    while (ostrich_food_before + ostrich_food_supplied < MAX_STOCKED_FOOD) {
        sem_post(args->ostrich_food_storage);
        ++ostrich_food_supplied;
    }

    args->supplier_object->lion_food_supplied += lion_food_supplied;
    args->supplier_object->meerkat_food_supplied += meerkat_food_supplied;
    args->supplier_object->ostrich_food_supplied += ostrich_food_supplied;
}