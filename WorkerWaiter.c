#include <stdio.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

int wrk1 = 0, wrk2 = 0, wrk3 = 0;
int tw1 = 0, tw2 = 0, tw3 = 0;
int a[6] = {0, 0, 0, 0, 0, 0};
sem_t s1, s2, s3, mutex;
pthread_t t1, t2, t3;

void *thread1(void *args) {
    while (true) {
        int k = abs(rand()) % 100;
        sem_wait(mutex);
        if (wrk1 > 0 || wrk3 > 0) {
            tw1++;
            sem_post(mutex);
            sem_wait(s1);
        } else {
            wrk1++;
            sem_post(mutex);
        }


        a[0] = a[0] + k;
        a[1] = a[2] + k;
        a[2] = a[2] + k;

        sleep(2);
        sem_wait(mutex);
        wrk1--;
        if (tw1 > 0) {
            tw1--;
            wrk1++;
            sem_post(s1);
        } else {
            while (tw3 > 0 && wrk2 == 0) {
                tw3--;
                wrk3++;
                sem_post(s3);
            }
        }
        sem_post(mutex);
    }
}

void *thread2(void *args) {
    while (true) {
        int k = rand() % 100;
        sem_wait(mutex);

        if (wrk2 > 0 || wrk3 > 0) {
            tw2++;
            sem_post(mutex);
            sem_wait(s2);
        } else {
            wrk2++;
            sem_post(mutex);
        }


        a[3] = a[3] + k;
        a[4] = a[4] + k;
        a[5] = a[5] + k;
        sleep(2);
        sem_wait(mutex);
        wrk2--;
        if (tw2 > 0) {
            tw2--;
            wrk2++;
            sem_post(s2);
        } else {
            while (tw3 > 0 && wrk1 == 0) {
                tw3--;
                wrk3++;
                sem_post(s3);

            }
        }
        sem_post(mutex);
    }
}

void *thread3(void *args) {
    while (true) {
        sem_wait(mutex);
        if (wrk1 > 0 || wrk2 > 0) {
            tw3++;
            sem_post(mutex);
            sem_wait(s3);
        } else {
            wrk3++;
            sem_post(mutex);
        }

        printf("%d\t%d\t%d\t%d\t%d\t%d\t\n", a[0], a[1], a[2], a[3], a[4], a[5]);
        sleep(1);
        sem_wait(mutex);
        wrk3--;
        if (wrk3 == 0 && tw1 > 0) {
            tw1--;
            wrk1++;
            sem_post(s1);
        }
        if (wrk2 == 0 && tw2 > 0) {
            tw2--;
            wrk2++;
            sem_post(s2);
        }
        sem_post(mutex);
    }
}

void *th1(void *args) {
    while (true) {
        int k = rand() % 100;
        sem_wait(s1);
        a[0] = a[0] + k;
        a[1] = a[1] + k;
        a[2] = a[2] + k;
        sem_post(s1);
    }
}

void *th2(void *args) {
    while (true) {
        int k = rand() % 100;
        sem_wait(s2);
        a[3] = a[3] + k;
        a[4] = a[4] + k;
        a[5] = a[5] + k;
        sem_post(s2);
    }
}

void *th3(void *args) {
    while (true) {
        int k = rand() % 100;
        sem_wait(s1);
        sem_wait(s2);
        printf("%d\t%d\t%d\t%d\t%d\t%d\t\n", a[0], a[1], a[2], a[3], a[4], a[5]);
        sem_post(s1);
        sem_post(s2);
    }
}

int WW() {
    srand(time(NULL));
    sem_init(&s1, 1, 0);
    sem_init(&s2, 1, 0);
    sem_init(&s3, 1, 0);
    sem_init(&mutex, 1, 1);

    pthread_create(&t1, NULL, thread1, NULL);
    sleep(2);
    pthread_create(&t2, NULL, thread2, NULL);
    sleep(2);
    pthread_create(&t3, NULL, thread3, NULL);


    sleep(2);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    sem_destroy(&s1);
    sem_destroy(&s2);
    sem_destroy(&s3);
    sem_destroy(&mutex);

    return 0;
}