

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "ProdCons.h"

void *print_hello(void *num) {
    printf("Ciao, sono il thread numero %d!\n", *((int *) num));
    pthread_exit(NULL);
}

int main(void) {
    prodConsS();
}