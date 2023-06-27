
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define N 10
#define LEFT ((i + N - 1) % N)
#define RIGHT ((i + 1) % N)
enum {
    THINKING, HUNGRY, EATING
} state[N];

sem_t state_mutex, philosopher_mutex[N];
pthread_t philosophers[N];

void think(int i);

void take_forks(int i);

void put_forks(int i);

void eat(int i);

void test(int i);

_Noreturn void *philosopher(void *id) {
    while (1) {
        think((int) id);
        take_forks((int) id);
        eat((int) id);
        put_forks((int) id);
    }

}

void eat(int i) {
    printf("Philosopher %d Eating...\n", i);
    sleep(.5 + rand() % 5);
}

void take_forks(int i) {
    sem_wait(state_mutex);
    state[i] = HUNGRY;
    test(i);
    sem_post(state_mutex);
    sem_wait(philosopher_mutex[i]);
}

void put_forks(int i) {
    sem_wait(state_mutex);
    state[i] = THINKING;
    test(LEFT);
    test(RIGHT);
    sem_post(state_mutex);
}

void test(int i) {
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[i] = EATING;
        sem_post(philosopher_mutex[i]);
    }
}

void think(int i) {
    printf("Philosopher %d Thinking...\n", i);
    sleep(.5 + rand() % 5);
}

int phil() {
    srand(time(NULL));
    sem_init(&state_mutex, 1, 1);
    for (int k = 0; k < N; k++) {
        sem_init((sem_t *) &state[k], 1, 0);
    }
    sleep(2);
    for (int k = 0; k < N; k++) {
        pthread_create(&philosophers[k], NULL, philosopher, (void *) k);
    }
    sleep(2);
    for (int k = 0; k < N; k++) {
        pthread_join(philosophers[k], NULL);
    }
    return 0;
}