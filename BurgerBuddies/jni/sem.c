#include "sem.h"
#include "bugen_bbc.h"

void Sem_init(sem_t *sem, unsigned int value) {
    if (sem_init(sem, O_CREAT | O_EXCL, value) < 0) {
        fprintf(stderr, "Sem_init error: %s", strerror(errno));
        exit(-1);
    }
}

void P(sem_t *sem) {
    sem_wait(sem);
}

void V(sem_t *sem) {
    sem_post(sem);
}
