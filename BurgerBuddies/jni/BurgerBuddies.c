#include "BurgerBuddies.h"
#include "bugen_bbc.h"
#include "sbuf.h"

void error() {
    printf("Usage: BBC #Cooks #Cashiers #Customers #RackSize\n");
    exit(-1);
}

sem_t customer_sem;
sem_t printf_sem;

sbuf_t customers;
sbuf_t burgers;

void* cook(void* _i) {
    int i = *(int*)_i;
    
    while (1) {
        randslp(0.2, 1.0);

        sbuf_insert(&burgers, i);

        P(&printf_sem);
        printf("Cook [%d] makes a burger.\n", i);
        V(&printf_sem);
    }
    pthread_exit(NULL);
}

void* cashier(void* _i) {
    int i = *(int*)_i;
    int customer, cook;
    
    while (1) {
        customer = sbuf_remove(&customers);

        P(&printf_sem);
        printf("Casher [%d] accepts an order from Customer [%d].\n", i,
               customer);
        V(&printf_sem);

        cook = sbuf_remove(&burgers);
        V(&customer_sem);

        P(&printf_sem);
        printf("Casher [%d] takes a burger form Cook [%d] to Customer [%d].\n",
               i, cook, customer);
        V(&printf_sem);
    }
    pthread_exit(NULL);
}

void* customer(void* _i) {
    int i = *(int*)_i;
    
    randslp(1.0, 10.0);

    sbuf_insert(&customers, i);

    P(&printf_sem);
    printf("Customer [%d] comes.\n", i);
    V(&printf_sem);

    pthread_exit(NULL);
}

void init() {
    Sem_init(&rand_sem, 1);
    Sem_init(&printf_sem, 1);
}

int main(int argc, char** argv) {
    int co = 0, ca = 0, cu = 0, ra = 0;
    pthread_t* tids;
    size_t idx = 0;
    int i;
    int* para;

    if (argc != 5) error();
    co = atoi(argv[1]);
    ca = atoi(argv[2]);
    cu = atoi(argv[3]);
    ra = atoi(argv[4]);
    if (co <= 0 || ca <= 0 || cu <= 0 || ra <= 0) error();

    init();
    Sem_init(&customer_sem, 0);
    sbuf_init(&customers, ca);
    sbuf_init(&burgers, ra);

    tids = malloc(sizeof(pthread_t) * (co + ca + cu));
    printf("Welcome to Bugen's Burger Buddies!\n");

    printf("Cooks [%d], Cashiers [%d], Customers [%d]\n\n", co, ca, cu);
    for (i = 0; i < cu; i++) {
        para = malloc(sizeof(int));
        *para = i + 1;
        pthread_create(&tids[idx], NULL, customer, para);
    }
    for (i = 0; i < co; i++) {
        para = malloc(sizeof(int));
        *para = i + 1;
        pthread_create(&tids[idx++], NULL, cook, para);
    }
    for (i = 0; i < ca; i++) {
        para = malloc(sizeof(int));
        *para = i + 1;
        pthread_create(&tids[idx++], NULL, cashier, para);
    }

    for (i = 0; i < cu; ++i) P(&customer_sem);
    printf("All customers are served! Done.\n");

    exit(0);
}
