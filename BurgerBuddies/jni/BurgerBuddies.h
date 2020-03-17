#ifndef BUGEN_BUGRERBUDDIES_H
#define BUGEN_BUGRERBUDDIES_H

#include "bugen_bbc.h"
#include "sem.h"

sem_t rand_sem;

int randint(int lo, int hi) {
    int ret;

    P(&rand_sem);
    static int flag = 0;
    if (!flag) {
        flag = 1;
        srand(time(NULL));
    }
    ret = rand() % (hi + 1 - lo) + lo;
    V(&rand_sem);

    return ret;
}

void randslp(float lo, float hi) {
    int cs = randint(lo * 10, hi * 10);
    while (cs--) usleep(100000);
}

#endif  // BUGEN_BUGRERBUDDIES_H
