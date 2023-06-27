#include <stdio.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUF_SIZE  100
int buf[BUF_SIZE];
sem_t sem_2, sem_3, sem_non3, sem_cons, sem_prod, sem_full, sem_empty;
pthread_t prod, cons;

void *produttore(void *arg) {
    int val, index_2 = 0, index_3 = 3, index_non3 = 1, index;
    while (1) {
        val = abs(rand()) % 100 + 1;
        sem_wait(sem_empty);
        sem_wait(sem_prod);
        if (val % 2 == 0) {
            index = index_2;
            buf[index_2] = val;
            sem_post(sem_2);
            index_2 = (index_2 + 2) % BUF_SIZE;
        } else if (val % 3 == 0) {
            index = index_3;
            buf[index_3] = val;
            sem_post(sem_3);
            do {
                index_3 = (index_3 + 3) % BUF_SIZE;
            } while (index_3 % 2 != 0);
        } else {
            index = index_non3;
            buf[index_non3] = val;
            sem_post(sem_non3);
            do {
                index_non3 = (index_non3 + 1) % BUF_SIZE;
            } while (index_non3 % 2 != 0 && index_non3 % 3 != 0);
        }
        sem_post(sem_prod);
        sem_post(sem_full);
        printf("PROD: wrote %d \t at index %d \n", val, index);
        sleep(rand() % 5);
    }
}

void *consumatore(void *arg) {
    int val_read, index = 0;
    while (1) {

        sem_wait(sem_full);
        sem_wait(sem_cons);
        if (index % 2 == 0) {
            sem_wait(sem_2);
        } else if (index % 3 == 0) {
            sem_wait(sem_3);
        } else {
            sem_wait(sem_non3);
        }
        val_read = buf[index];
        index = (index + 1) % BUF_SIZE;
        sem_post(sem_cons);
        sem_post(sem_empty);

        printf("CONS: read %d \t at index %d\n", val_read, index - 1);

        sleep(rand() % 5);
    }
}

int PC() {

    srand(time(NULL));   // Initialization, should only be called once.
    sem_init(&sem_2, 1, 0);
    sem_init(&sem_3, 1, 0);
    sem_init(&sem_non3, 1, 0);
    sem_init(&sem_cons, 1, 1);
    sem_init(&sem_prod, 1, 1);
    sem_init(&sem_full, 1, 0);
    sem_init(&sem_empty, 1, BUF_SIZE);

    sleep(2);

    pthread_create(&prod, NULL, produttore, NULL);
    sleep(2);
    pthread_create(&cons, NULL, consumatore, NULL);
    sleep(2);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    sem_destroy(&sem_2);
    sem_destroy(&sem_3);
    sem_destroy(&sem_non3);
    sem_destroy(&sem_cons);
    sem_destroy(&sem_prod);
    sem_destroy(&sem_full);
    sem_destroy(&sem_empty);
    return 0;
}
