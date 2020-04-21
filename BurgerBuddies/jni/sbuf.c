// A simple thread-safe FIFO buffer library inspired by CSAPP

//
// Created by Bugen Zhao on 2020/3/7.
//
// A thread-safe FIFO buffer, inspired by CS:APP

#include "sbuf.h"

#include "bugen_bbc.h"
#include "sem.h"

// Create an empty buffer with max size n
// @sp: pointer to buffer
// @n: max size
void sbuf_init(sbuf_t *sp, int n) {
    sp->buf = calloc(n, sizeof(int));
    sp->n = n;                // Max size
    sp->front = sp->rear = 0; // Front and read of the circular queue
    Sem_init(&sp->mutex, 1);  // Lock for accessing
    Sem_init(&sp->slots, n);  // Available slots
    Sem_init(&sp->items, 0);  // Used slots
}

// Deinit the buffer
// @sp: pointer to buffer
void sbuf_deinit(sbuf_t *sp) {
    free(sp->buf);
}

// Insert an item to the rear of buffer
// @sp: pointer to buffer
// @item: item to input
void sbuf_insert(sbuf_t *sp, int item) {
    P(&sp->slots);                          // Wait for any available slot
    P(&sp->mutex);                          // Lock!
    sp->buf[(++sp->rear) % (sp->n)] = item; // Insert item
    V(&sp->mutex);                          // Unlock!
    V(&sp->items);                          // One more used slot
}

// Remove then return the first item from buffer
// @sp: pointer to buffer
int sbuf_remove(sbuf_t *sp) {
    int item;
    P(&sp->items);                           // Wait for any available items
    P(&sp->mutex);                           // Lock!
    item = sp->buf[(++sp->front) % (sp->n)]; // Remove item
    V(&sp->mutex);                           // Unlock!
    V(&sp->slots);                           // One more available slot
    return item;
}
