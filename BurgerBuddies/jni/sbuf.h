//
// Created by Bugen Zhao on 2020/3/7.
//

#ifndef BUGENCSAPP3E_SBUF_H
#define BUGENCSAPP3E_SBUF_H

#include "bugen_bbc.h"

// A thread-safe FIFO buffer, inspired by CS:APP
typedef struct {
    int *buf;     // Buffer
    int n;        // Max size
    int front;    // Front pointer of circular queue
    int rear;     // Rear pointer of circular queue
    sem_t mutex;  // Semaphore for accessing
    sem_t slots;  // Available slots
    sem_t items;  // Used slots
} sbuf_t;

void sbuf_init(sbuf_t *sp, int n);

void sbuf_deinit(sbuf_t *sp);

void sbuf_insert(sbuf_t *sp, int item);

int sbuf_remove(sbuf_t *sp);

#endif  // BUGENCSAPP3E_SBUF_H
