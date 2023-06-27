#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define WAITING_DIM 20

sem_t barber, clienti_attesa, mutexx;
pthread_t b, c;

int posti_d = 20;
int posti_occupati = 0;

void taglio() {
    printf("Barber effettua taglio\n");
    sleep(2);
}

void ricevetaglio() {
    printf("Cliente riceve taglio\n");
    sleep(2);
}

void *barber_F(void *args) {
    while (1) {
        sem_wait(clienti_attesa);
        sem_wait(mutexx);
        posti_occupati--;
        sem_post(mutexx);
        sem_wait(barber);
        taglio();
    }
}

void *cliente(void *args) {
    sem_wait(mutexx);
    if (posti_occupati < posti_d) {
        printf("Cliente entra\n");
        posti_occupati++;
        sem_post(clienti_attesa);
        sem_post(mutexx);
        sem_post(barber);
        ricevetaglio();
    } else {
        sem_post(mutexx);
    }
}


int SB() {
    sem_init(&barber, 1, 0);
    sem_init(&clienti_attesa, 1, 0);
    sem_init(&mutexx, 1, 1);
    sleep(2);

    pthread_create(&b, NULL, barber_F, NULL);
    sleep(2);
    pthread_create(&c, NULL, cliente, NULL);

    sleep(2);
    pthread_join(b, NULL);
    pthread_join(c, NULL);
    sem_destroy(&barber);
    sem_destroy(&clienti_attesa);
    sem_destroy(&mutexx);
    return 0;
}