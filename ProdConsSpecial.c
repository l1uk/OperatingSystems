#include <semaphore.h>
#include <stdlib.h>

int productible_items = 20, num_producted_elements = 0, num_cons_can_go = 5;
sem_t prod_mutex, cons_norm_mutex, cons_spec_mutex, mutex_general;
int prodWaiting = 0, consWaiting = 0, consSpecialWaiting = 0;

_Noreturn void *prod_special(void *args) {
    while (1) {
        sem_wait(mutex_general);
        if (productible_items == 0) {
            prodWaiting++;
            sem_post(mutex_general);
            sem_wait(prod_mutex);
        } else {
            productible_items--;
            sem_post(mutex_general);
        }
        // produzione
        sem_wait(mutex_general);
        if (num_cons_can_go > 0 && consWaiting > 0) {
            consWaiting--;
            sem_post(cons_norm_mutex);
        } else {
            if (consSpecialWaiting > 0 && num_producted_elements == 1) {
                consSpecialWaiting--;
                num_producted_elements = 0;
                num_cons_can_go = 5;
                sem_post(cons_spec_mutex);
            } else {
                num_producted_elements++;
            }
        }
        sem_post(mutex_general);
    }
}

_Noreturn void *cons_normal(void *args) {
    while (1) {
        sem_wait(mutex_general);
        if (num_producted_elements == 0 || num_cons_can_go == 0) {
            consWaiting++;
            sem_post(mutex_general);
            sem_wait(cons_norm_mutex);
            sem_wait(mutex_general);
        } else {
            num_producted_elements--;
        }
        if (consSpecialWaiting > 0) {
            num_cons_can_go--;
        }
        sem_post(mutex_general);
        //consumazione
        sem_wait(mutex_general);
        if (prodWaiting > 0) {
            prodWaiting--;
            sem_post(prod_mutex);
        } else {
            productible_items++;
        }
        sem_post(mutex_general);
    }
}

_Noreturn void *cons_special(void *args) {
    while (1) {
        sem_wait(mutex_general);
        if (num_producted_elements <= 1) {
            consSpecialWaiting++;
            sem_post(mutex_general);
            sem_wait(cons_spec_mutex);
        } else {
            num_producted_elements -= 2;
            sem_post(mutex_general);
        }
        //consumazione di due item
        sem_wait(mutex_general);
        if (prodWaiting > 1) {
            prodWaiting -= 2;
            sem_post(prod_mutex);
            sem_post(prod_mutex);
        } else if (prodWaiting == 1) {
            prodWaiting--;
            sem_post(prod_mutex);
            productible_items++;
        } else {
            productible_items += 2;
        }
        sem_post(mutex_general);
    }
}

int ProdConsSpecial() {

    sem_init(&mutex_general, 1, 1);
    sem_init(&prod_mutex, 1, 0);
    sem_init(&cons_norm_mutex, 1, 0);
    sem_init(&cons_spec_mutex, 1, 0);


    return 0;
}