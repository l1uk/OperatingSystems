#include <semaphore.h>

#define VENDING_MACHINE_SIZE 50
#define MAX_NUM_CRACKERS 3

enum {
    BISCUIT, CRACKER, EMPTY
} vending_machine[VENDING_MACHINE_SIZE];
int num_cracker = 0, num_biscuits = 0, capacity = VENDING_MACHINE_SIZE;
int biscuit_vendor_waiting = 0, crackers_vendor_waiting = 0, vendor_index = 0, consumer_biscuit_index = 0, consumer_cracker_index = 0;
int biscuit_consumer_waiting = 0, cracker_consumer_waiting = 0;
int num_vendors_putting_biscuit = 0, num_vendors_putting_cracker = 0;
sem_t generic_mutex, biscuit_vendor_mutex, crackers_vendor_mutex, biscuit_consumer_mutex, crackers_consumer_mutex;

_Noreturn void *biscuit_furnisher(void *args) {
    while (1) {
        sem_wait(generic_mutex);
        if (capacity == 0 || biscuit_vendor_waiting > 0 || num_vendors_putting_biscuit > 0) {
            biscuit_vendor_waiting++;
            sem_post(generic_mutex);
            sem_wait(biscuit_vendor_mutex);
        } else {
            sem_post(generic_mutex);
        }

        num_vendors_putting_biscuit++;
        vending_machine[vendor_index] = BISCUIT;
        vendor_index = (vendor_index + 1) % VENDING_MACHINE_SIZE;
        capacity--;
        sem_wait(generic_mutex);
        num_vendors_putting_biscuit--;
        if (crackers_vendor_waiting > 0 && capacity > 0 &&
            !(num_cracker >= num_biscuits && num_cracker >= MAX_NUM_CRACKERS)) {
            crackers_vendor_waiting--;
            sem_post(crackers_vendor_mutex);
        } else if (biscuit_vendor_waiting > 0 && capacity > 0) {
            biscuit_vendor_waiting--;
            sem_post(generic_mutex);
            sem_post(biscuit_vendor_mutex);
        }
        sem_post(generic_mutex);

    }
}

_Noreturn void *crackers_furnisher(void *args) {
    while (1) {
        sem_wait(generic_mutex);
        if (capacity == 0 || crackers_vendor_waiting > 0 || num_vendors_putting_cracker > 0 ||
            (num_cracker >= num_biscuits && num_cracker >= MAX_NUM_CRACKERS)) {
            crackers_vendor_waiting++;
            sem_post(generic_mutex);
            sem_wait(crackers_vendor_mutex);
        } else {
            sem_post(generic_mutex);
        }

        num_vendors_putting_cracker++;
        vending_machine[vendor_index] = CRACKER;
        vendor_index = (vendor_index + 1) % VENDING_MACHINE_SIZE;

        sem_wait(generic_mutex);
        if (biscuit_vendor_waiting > 0 && capacity > 0) {
            biscuit_vendor_waiting--;
            sem_post(generic_mutex);
            sem_post(biscuit_vendor_mutex);
        }

    }
}

_Noreturn void *biscuit_consumer(void *args) {
    sem_wait(generic_mutex);
    if (num_biscuits > 0) {
        sem_post(generic_mutex);
        sem_wait(biscuit_consumer_mutex);

        while (vending_machine[consumer_biscuit_index] != BISCUIT)
            consumer_biscuit_index = consumer_biscuit_index + 1 % VENDING_MACHINE_SIZE;
        vending_machine[consumer_biscuit_index] = EMPTY;
        sem_post(biscuit_consumer_mutex);
        sem_wait(generic_mutex);
        num_biscuits--;
        capacity++;
        sem_post(generic_mutex);
    } else
        sem_post(generic_mutex);
}

_Noreturn void *cracker_consumer(void *args) {
    sem_wait(generic_mutex);
    if (num_cracker > 0) {
        sem_post(generic_mutex);
        sem_wait(crackers_consumer_mutex);
        while (vending_machine[consumer_cracker_index] != CRACKER)
            consumer_cracker_index = consumer_cracker_index + 1 % VENDING_MACHINE_SIZE;
        vending_machine[consumer_cracker_index] = EMPTY;
        sem_post(crackers_consumer_mutex);
        sem_wait(generic_mutex);
        num_cracker--;
        capacity++;
        sem_post(generic_mutex);
    } else
        sem_post(generic_mutex);
}