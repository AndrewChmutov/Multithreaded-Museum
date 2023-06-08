#include "multivisit.h"
#include "render.h"
#include <stdlib.h>
#include <unistd.h>


void visitor_B() {
    // enter the queue
    pthread_mutex_lock(&mut);
    int idx = render('B', -1, 1);
    pthread_mutex_unlock(&mut);

    usleep(rand() % 500000);

    pthread_mutex_lock(&mut);
    render('B', idx, 0);
    idx = render('A', -2, 1);
    render('A', idx, 1);
    pthread_mutex_unlock(&mut);
    // visitor_A(1);
    pthread_mutex_lock(&mut_b);
    freeB++;
    pthread_cond_signal(&cond_b);
    pthread_mutex_unlock(&mut_b);
}

void visitor_A() {
    // if room A is full - wait
    usleep(rand() % 500000);
    pthread_mutex_lock(&mut_a);
    while (freeA == 1) 
        pthread_cond_wait(&cond_a, &mut_a);
    freeA--;
    pthread_mutex_unlock(&mut_a);

    usleep(rand() % 500000);

    // find the place to fit and show
    pthread_mutex_lock(&mut);
    int idx = render('A', -1, 1);
    pthread_mutex_unlock(&mut);


    usleep(rand() % 50000);

    // randomly decides
    // either to enter room B
    // or leave
    if (rand() % 3) {
        // enter room B
        pthread_mutex_lock(&mut_b);
        while (freeB == 0)
            pthread_cond_wait(&cond_b, &mut_b);
        freeB--;
        pthread_mutex_unlock(&mut_b);

        // show it
        pthread_mutex_lock(&mut);
        render('A', idx, 0);
        pthread_mutex_unlock(&mut);

        // leave room A
        pthread_mutex_lock(&mut_a);
        freeA++;
        pthread_cond_signal(&cond_a);
        pthread_mutex_unlock(&mut_a);

        visitor_B();
    }
    else {
        // show that visitor is leaving
        pthread_mutex_lock(&mut);
        render('A', idx, 1);
        pthread_mutex_unlock(&mut);

        // leave room A
        pthread_mutex_lock(&mut_a);
        freeA++;
        pthread_cond_signal(&cond_a);
        pthread_mutex_unlock(&mut_a);
    }
}

void* worker(void *arg) {
    usleep(rand() % 2000000 + 1000000);

    // queue before museum
    pthread_mutex_lock(&mut_out);
    while (total_allowed == 0)
        pthread_cond_wait(&cond_out, &mut_out);
    total_allowed--;
    pthread_mutex_unlock(&mut_out);

    visitor_A();

    // leave museum
    pthread_mutex_lock(&mut_out);
    total_allowed++;
    pthread_cond_signal(&cond_out);
    pthread_mutex_unlock(&mut_out);

    pthread_exit(NULL);
}

