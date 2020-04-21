// Main code of problem 4

#include "BurgerBuddies.h"

#include "bugen_bbc.h"
#include "sbuf.h"

// Throw an error
void error(char* argv0) {
    printf("Usage: %s #Cooks #Cashiers #Customers #RackSize\n", argv0);
    exit(-1);
}

// Record how many customers have we served
// For the main progrem to determine whether to exit
sem_t customer_sem;
// A lock for printf
sem_t printf_sem;

// Two FIFO buffers, which store the # of instances
sbuf_t customers;
sbuf_t burgers;

// Cook thread
// @_i: pointer to the number
void* cook(void* _i) {
    int i = *(int*)_i;

    while (1) {
        // Random sleep for a while
        randslp(0.2, 1.0);
        // Make a burger
        sbuf_insert(&burgers, i);
        // Print info
        P(&printf_sem);
        printf("Cook [%d] makes a burger.\n", i);
        V(&printf_sem);
    }
    pthread_exit(NULL);
}

// Cashier thread
// @_i: pointer to the number
void* cashier(void* _i) {
    int i = *(int*)_i;
    int customer, cook;

    while (1) {
        // Accept a customer
        customer = sbuf_remove(&customers);
        // Print info
        P(&printf_sem);
        printf("Casher [%d] accepts an order from Customer [%d].\n", i,
               customer);
        V(&printf_sem);
        // Get a burger
        cook = sbuf_remove(&burgers);
        // Yep! We've served one!
        V(&customer_sem);
        // Print info
        P(&printf_sem);
        printf("Casher [%d] takes a burger form Cook [%d] to Customer [%d].\n",
               i, cook, customer);
        V(&printf_sem);
    }
    pthread_exit(NULL);
}

// Customer thread
// @_i: pointer to the number
void* customer(void* _i) {
    int i = *(int*)_i;
    // Random sleep for a while
    randslp(1.0, 10.0);
    // Enter the burger buddies!
    sbuf_insert(&customers, i);
    // Print info
    P(&printf_sem);
    printf("Customer [%d] comes.\n", i);
    V(&printf_sem);

    // The customer does not leave.
    // In fact, he is waiting in the `customers` buffer
    pthread_exit(NULL);
}

// Initialization of key semaphores
void init() {
    // For random sleep in `BurgerBuddies.h`
    Sem_init(&rand_sem, 1);
    // For safer `printf()`
    Sem_init(&printf_sem, 1);
}

int main(int argc, char** argv) {
    // Cook, Cashier, Customer, RackSize
    int co = 0, ca = 0, cu = 0, ra = 0;
    pthread_t tid;
    int i;
    int* para;

    // Parse the arguments
    if (argc != 5) error(argv[0]);
    co = atoi(argv[1]);
    ca = atoi(argv[2]);
    cu = atoi(argv[3]);
    ra = atoi(argv[4]);
    if (co <= 0 || ca <= 0 || cu <= 0 || ra <= 0) error(argv[0]);

    // Init semaphores
    init();
    Sem_init(&customer_sem, 0);

    // Init buffers with their capacity
    sbuf_init(&customers, ca);
    sbuf_init(&burgers, ra);

    printf("Welcome to Bugen's Burger Buddies!\n");
    printf("Cooks [%d], Cashiers [%d], Customers [%d]\n\n", co, ca, cu);

    // Create threads
    for (i = 0; i < cu; i++) {
        // Allocate space for argument (#)
        // `free()` is not needed,
        //  since we will do this for just (cu+co+ca) -> very limited times
        para = malloc(sizeof(int));
        *para = i + 1;
        pthread_create(&tid, NULL, customer, para);
    }
    for (i = 0; i < co; i++) {
        para = malloc(sizeof(int));
        *para = i + 1;
        pthread_create(&tid, NULL, cook, para);
    }
    for (i = 0; i < ca; i++) {
        para = malloc(sizeof(int));
        *para = i + 1;
        pthread_create(&tid, NULL, cashier, para);
    }

    // Wait for all `cu` customers to be serverd
    for (i = 0; i < cu; ++i) P(&customer_sem);
    printf("All customers are served! Done.\n");

    exit(0);
}
