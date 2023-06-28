#include <semaphore.h>
#include <stdlib.h>

#define SIZEn 3
int buff3r[SIZEn], writer_writing[SIZEn], writer_waiting[SIZEn], reader_reading = 0, reader_waiting = 0, writers_writing = 0, writers_waiting = 0;
int numAccess[SIZEn], numReads;
sem_t general_mutex, writer_waiting_mutex[SIZEn], reader;

_Noreturn void *writer(void *args) {
    while (1) {
        int n = rand() % 3;
        int val = rand() % 100;
        sem_wait(general_mutex);
        if (writer_writing[n] > 0 || reader_reading > 0 || numAccess[n] >= 4) {
            writers_waiting++;
            writer_waiting[n]++;
            sem_post(general_mutex);
            sem_wait(writer_waiting_mutex[n]);
        }
        writer_writing[n]++;
        writers_writing++;
        numAccess[n]++;
        numReads = 0;
        buff3r[n] = val;

        sem_wait(general_mutex);
        writer_writing[n]--;
        writers_writing--;
        if (reader_waiting > 0) {
            reader_waiting--;
            sem_post(general_mutex);
            sem_post(reader);
        } else if (writer_waiting[n] > 0 && numAccess[n] < 4) {
            writer_waiting[n]--;
            writers_waiting--;
            sem_post(general_mutex);
            sem_post(writer_waiting_mutex[n]);
        }
    }
}

_Noreturn void *r34d3r(void *args) {
    while (1) {
        sem_wait(general_mutex);
        if (writers_writing > 0 || numReads > 10) {
            reader_waiting++;
            sem_post(general_mutex);
            sem_wait(reader);
        }

        sem_wait(general_mutex);
        reader_reading++;
        numReads++;
        sem_post(general_mutex);

        int val = buff3r[0] + buff3r[1] + buff3r[2];

        sem_wait(general_mutex);
        reader_reading--;
        if (numReads >= 10 && writer_waiting[0] + writer_waiting[1] + writer_waiting[2] > 0) {
            for (int i = 0; i < SIZEn; i++) {
                if (writer_waiting[i] > 0) {
                    writer_waiting[i]--;
                    sem_post(writer_waiting_mutex[i]);
                }
            }
        }

    }
}