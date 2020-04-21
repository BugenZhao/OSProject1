// Main code of problem 4

#include "BurgerBuddies.h"

#include "bugen_bbc.h"
#include "sbuf.h"

// Throw a usage error
inline void usage_error(char* argv0) {
    printf("Usage: %s #Cooks #Cashiers #Customers #RackSize\n", argv0);
    exit(-1);
}

// Throw a runtime error
inline void thread_error(void) {
    fprintf(stderr, "Failed to create thread.\n");
    exit(-2);
}

// Record how many customers have we served
// For the main progrem to determine whether to exit
sem_t customer_sem;
// A lock for printf
sem_t printf_sem;

// Two FIFO buffers, which store the # of instances
sbuf_t customers;
sbuf_t burgers;

#if 1
#define printf_safe(fmt, args...) \
    do {                          \
        P(&printf_sem);           \
        printf(fmt, args);        \
        V(&printf_sem);           \
    } while (0)
#else
#define printf_safe(fmt, args...) printf(fmt, args)
#endif

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
        printf_safe("Cook [%d] makes a burger.\n", i);
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
        printf_safe("Casher [%d] accepts an order from Customer [%d].\n", i,
                    customer);
        // Get a burger
        cook = sbuf_remove(&burgers);
        // Yep! We've served one!
        V(&customer_sem);
        // Print info
        printf_safe(
            "Casher [%d] takes a burger form Cook [%d] to Customer [%d].\n", i,
            cook, customer);
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
    printf_safe("Customer [%d] comes.\n", i);

    // The customer does not leave.
    // In fact, he is waiting in the `customers` buffer
    pthread_exit(NULL);
}

// Initialization of key semaphores
void init() {
    // For random sleep in `BurgerBuddies.h`
    Sem_init(&rand_sem, 1);
    // For `printf_safe()`
    Sem_init(&printf_sem, 1);
}

int main(int argc, char** argv) {
    // Cook, Cashier, Customer, RackSize
    int co = 0, ca = 0, cu = 0, ra = 0;
    pthread_t tid;
    int i;
    int* para;

    // Parse the arguments
    if (argc != 5) usage_error(argv[0]);
    co = atoi(argv[1]);
    ca = atoi(argv[2]);
    cu = atoi(argv[3]);
    ra = atoi(argv[4]);
    if (co <= 0 || ca <= 0 || cu <= 0 || ra <= 0) usage_error(argv[0]);

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
        // No that `free()` is not needed,
        //  since we will do this for just (cu+co+ca) -> very limited times
        if ((para = malloc(sizeof(int))) == NULL) thread_error();
        *para = i + 1;
        if (pthread_create(&tid, NULL, customer, para)) thread_error();
    }
    for (i = 0; i < co; i++) {
        if ((para = malloc(sizeof(int))) == NULL) thread_error();
        *para = i + 1;
        if (pthread_create(&tid, NULL, cook, para)) thread_error();
    }
    for (i = 0; i < ca; i++) {
        if ((para = malloc(sizeof(int))) == NULL) thread_error();
        *para = i + 1;
        if (pthread_create(&tid, NULL, cashier, para)) thread_error();
    }

    // Wait for all customers to be serverd
    for (i = 0; i < cu; ++i) P(&customer_sem);
    printf("All customers are served! Done.\n");

    // According to POSIX, exit() of main thread will safely terminate others
    exit(0);
}
