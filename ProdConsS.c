
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define BUF_S 20
int buff[BUF_S];
sem_t pro, con, ful, empt, prod_num, pr_special;
pthread_t pr, co;
int prod_number;

_Noreturn void *pr0d(void *args) {
    int index = 0;
    while (true) {
        int val = rand() % 100;
        sem_wait(empt);
        sem_wait(pro);

        buff[index] = val;
        printf("Productor wrote %d at %d\n", val, index);
        sem_wait(prod_num);
        prod_number += 1;
        sem_post(prod_num);
        index = (index + 1) % BUF_S;

        sem_post(pro);
        sem_post(ful);

    }
}

_Noreturn void *c0ns(void *args) {
    int index = 0;
    while (true) {
        int val;
        sem_wait(ful);
        sem_wait(con);

        val = buff[index];
        index = (index + 1) % BUF_S;
        printf("Consumer read %d at %d\n", val, index);
        sem_post(pro);
        sem_post(empt);
    }
}

_Noreturn void *c0ns_special(void *args) {
    int index = 0;
    while (true) {
        sem_wait(prod_num);
        if (prod_number < 2) {
            sem_wait(pr_special);
        }
        int val;
        sem_wait(con);

        val = buff[index];
        index = (index + 1) % BUF_S;
        printf("Consumer read %d at %d\n", val, index);
        sem_post(pro);
        sem_post(empt);
        sem_post(empt);
    }
}

int prodConsS() {
    srand(time(NULL));
    sem_init(&pro, 1, 1);
    sem_init(&pr_special, 1, 1);
    sem_init(&con, 1, 0);
    sem_init(&ful, 1, 0);
    sem_init(&empt, 1, BUF_S);
    sem_init(&prod_num, 1, 1);

    pthread_create(&pr, NULL, pr0d, NULL);
    sleep(2);
    pthread_create(&co, NULL, c0ns, NULL);
    sleep(2);

    pthread_join(pr, NULL);
    pthread_join(co, NULL);

    sem_destroy(pro);
    sem_destroy(con);
    sem_destroy(ful);
    sem_destroy(empt);

    return 0;
}