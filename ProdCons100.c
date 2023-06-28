#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 100

int buffer[BUF_SIZE];
sem_t pari, dispari, mutex_generic, cons_spec, cons_norm;
int productible_pari = 50, productible_dispari = 50, producted_pari = 0, producted_dispari = 0;
int producing_pari = 0, producing_dispari = 0, prod_pari_waiting = 0, prod_dispari_waiting = 0, cons_norm_waiting = 0, cons_spec_waiting = 0;
int consuming_norm = 0, consuming_spec = 0;
int pari_index = 0, dispari_index = 0, offset_dispari = BUF_SIZE / 2, pari_index_cons = 0, dispari_index_cons = 0;

_Noreturn void *prod0c3r(void *args) {
    while (1) {
        int val = rand() % BUF_SIZE;
        sem_wait(mutex_generic);
        if ((val % 2 == 0 && productible_pari == 0) || producing_pari > 0) {
            prod_pari_waiting++;
            sem_post(mutex_generic);
            sem_wait(pari);
        } else if ((val % 2 != 0 && productible_dispari == 0) || producing_dispari > 0) {
            prod_dispari_waiting++;
            sem_post(mutex_generic);
            sem_wait(dispari);
        }

        sem_wait(mutex_generic);

        if (val % 2 == 0) {
            productible_pari--;
            producted_pari++;
            producing_pari++;
        } else {
            productible_dispari--;
            producted_dispari++;
            producing_dispari++;
        }
        sem_post(mutex_generic);

        if (val % 2 == 0) {
            buffer[pari_index] = val;
            pari_index = (pari_index + 1) % (BUF_SIZE / 2);
        } else {
            buffer[dispari_index + offset_dispari] = val;
            dispari_index = (dispari_index + 1) % (BUF_SIZE / 2);
        }

        sem_wait(mutex_generic);
        if (val % 2 == 0) {
            producing_pari--;
        } else {
            producing_dispari--;
        }
        if (cons_spec_waiting > 0 && producted_pari >= 20) {
            cons_spec_waiting--;
            sem_post(mutex_generic);
            sem_post(cons_spec);
        } else if (cons_norm_waiting > 0 && (producted_pari > 0 || producted_dispari > 0)) {
            cons_norm_waiting--;
            sem_post(mutex_generic);
            sem_post(cons_norm);
        }


    }
}

_Noreturn void *c0nsum3r(void *args) {
    while (1) {
        sem_wait(mutex_generic);
        if ((producted_pari == 0 || producted_dispari == 0) || consuming_norm > 0 || consuming_spec > 0) {
            cons_norm_waiting++;
            sem_post(mutex_generic);
            sem_wait(cons_norm);
        }

        sem_wait(mutex_generic);
        consuming_norm++;

        if (producted_pari > 0) {
            producted_pari--;
            productible_pari++;
            sem_post(mutex_generic);
            //READ
            pari_index_cons = (pari_index_cons + 1) % (BUF_SIZE / 2);
        } else {
            producted_dispari--;
            productible_dispari++;
            sem_post(mutex_generic);
            //READ
            dispari_index_cons = (dispari_index_cons + 1) % (BUF_SIZE / 2);
        }

        sem_wait(mutex_generic);
        consuming_norm--;
        if (prod_pari_waiting > 0 && productible_pari > 0) {
            prod_pari_waiting--;
            sem_post(mutex_generic);
            sem_post(pari);
        } else if (prod_dispari_waiting > 0 && productible_dispari > 0) {
            prod_dispari_waiting--;
            sem_post(mutex_generic);
            sem_post(dispari);
        }

    }
}

_Noreturn void *c0nsum3r_sp3c1al(void *args) {
    while (1) {
        sem_wait(mutex_generic);
        if (producted_pari < 20 || consuming_spec > 0) {
            cons_spec_waiting++;
            sem_post(mutex_generic);
            sem_wait(cons_spec);
        }

        sem_wait(mutex_generic);
        consuming_spec++;
        sem_post(mutex_generic);

        producted_pari -= 20;
        productible_pari += 20;
        //READ
        pari_index_cons = (pari_index_cons + 20) % (BUF_SIZE / 2);

        sem_wait(mutex_generic);
        consuming_spec--;
        if (prod_pari_waiting > 0 && productible_pari > 0) {
            prod_pari_waiting--;
            sem_post(mutex_generic);
            sem_post(pari);
        } else if (cons_norm_waiting > 0 && (producted_pari > 0 || producted_dispari > 0)) {
            cons_norm_waiting--;
            sem_post(mutex_generic);
            sem_post(cons_norm);
        }
    }
}

int PC100() {

    sem_init(&pari, 1, 1);
    sem_init(&dispari, 1, 1);
    sem_init(&mutex_generic, 1, 1);
    sem_init(&cons_spec, 1, 0);
    sem_init(&cons_norm, 1, 0);

    sleep(2);


    return 0;
}