#ifndef BUGEN_SEM_H
#define BUGEN_SEM_H

#include <semaphore.h>
// See `sem.c` for comments

void Sem_init(sem_t *sem, unsigned int value);

void P(sem_t *sem);

void V(sem_t *sem);

#endif  // BUGEN_SEM_H
