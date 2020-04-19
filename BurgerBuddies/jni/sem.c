#include "sem.h"
#include "bugen_bbc.h"

// A wrapper function to init an anonymous semaphore
// @sem: pointer to semaphore
// @value: initial value
void Sem_init(sem_t *sem, unsigned int value) {
    if (sem_init(sem, O_CREAT | O_EXCL, value) < 0) {
        fprintf(stderr, "Sem_init error: %s", strerror(errno));
        exit(-1);
    }
}

// A wrapper function of `sem_wait()`
void P(sem_t *sem) {
    sem_wait(sem);
}

// A wrapper function of `sem_post()`
void V(sem_t *sem) {
    sem_post(sem);
}
