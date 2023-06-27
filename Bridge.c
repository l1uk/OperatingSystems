#include <stdio.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

int nNorth = 0, nSouth = 0;
int waitingNorth = 0, waitingSouth = 0;

sem_t variable_mutex, bridge_mutex;
sem_t fromNorth, fromSouth;
pthread_t n, s;

void *fromNorthToSouth(void *args) {

    while (true) {
        sem_wait(variable_mutex);
        if (nNorth > 0 || nSouth > 0) {
            waitingNorth += 1;
            sem_post(variable_mutex);
            sem_wait(fromNorth);
        } else {
            nNorth += 1;
            sem_post(variable_mutex);
        }

        sem_wait(bridge_mutex);
        printf("Crossing from north...\n");
        sleep(rand() % 5);
        sem_post(bridge_mutex);
        sem_wait(variable_mutex);
        nNorth--;
        if (waitingSouth > 0) {
            waitingSouth--;
            sem_post(variable_mutex);
            sem_post(fromSouth);
        } else if (waitingNorth > 0) {
            waitingNorth--;
            sem_post(variable_mutex);
            sem_post(fromNorth);
        }
    }
}

void *fromSouthToNorth(void *args) {

    while (true) {
        sem_wait(variable_mutex);
        if (nNorth > 0 || nSouth > 0) {
            waitingSouth += 1;
            sem_post(variable_mutex);
            sem_wait(fromSouth);
        } else {
            nSouth += 1;
            sem_post(variable_mutex);
        }
        sem_wait(bridge_mutex);
        printf("Crossing from south...\n");
        sleep(rand() % 5);
        sem_post(bridge_mutex);
        sem_wait(variable_mutex);
        nSouth--;
        if (waitingNorth > 0) {
            waitingNorth--;
            sem_post(variable_mutex);
            sem_post(fromNorth);
        } else if (waitingSouth > 0) {
            waitingSouth--;
            sem_post(variable_mutex);
            sem_post(fromSouth);
        }
    }
}


int BRIDGE() {
    srand(time(NULL));
    sem_init(&variable_mutex, 1, 1);
    sem_init(&bridge_mutex, 1, 1);
    sem_init(&fromNorth, 1, 0);
    sem_init(&fromSouth, 1, 0);


    pthread_create(&n, NULL, fromNorthToSouth, NULL);
    sleep(2);
    pthread_create(&s, NULL, fromSouthToNorth, NULL);
    sleep(2);

    pthread_join(n, NULL);
    pthread_join(s, NULL);

    sem_destroy(variable_mutex);
    sem_destroy(bridge_mutex);
    sem_destroy(fromNorth);
    sem_destroy(fromSouth);

    return 0;
}