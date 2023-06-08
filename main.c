#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "render.h"

const int width = 9;

pthread_mutex_t mut;
pthread_mutex_t mut_a;
pthread_mutex_t mut_b;
pthread_mutex_t mut_out;
pthread_cond_t cond_a;
pthread_cond_t cond_b;
pthread_cond_t cond_out;
int Na, Nb, freeA, freeB, total_allowed;
int *a, *b;
int to_enter;

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
    usleep(random() % 500000);
    pthread_mutex_lock(&mut_a);
    while (freeA == 1) 
        pthread_cond_wait(&cond_a, &mut_a);
    freeA--;
    pthread_mutex_unlock(&mut_a);

    usleep(rand() % 500000);

    pthread_mutex_lock(&mut);
    int idx = render('A', -1, 1);
    pthread_mutex_unlock(&mut);


    usleep(rand() % 50000);
    if (1) {

        pthread_mutex_lock(&mut_b);
        while (freeB == 0)
            pthread_cond_wait(&cond_b, &mut_b);
        freeB--;
        pthread_mutex_unlock(&mut_b);

        pthread_mutex_lock(&mut);
        render('A', idx, 0);
        pthread_mutex_unlock(&mut);

        pthread_mutex_lock(&mut_a);
        freeA++;
        pthread_cond_signal(&cond_a);
        pthread_mutex_unlock(&mut_a);

        visitor_B();
        
    }
    else {

        pthread_mutex_lock(&mut);
        render('A', idx, 1);
        pthread_mutex_unlock(&mut);

        pthread_mutex_lock(&mut_a);
        freeA++;
        pthread_cond_signal(&cond_a);
        pthread_mutex_unlock(&mut_a);
    }
}

void* worker(void *arg) {
    usleep(rand() % 2000000 + 1000000);
    pthread_mutex_lock(&mut_out);
    while (total_allowed == 0)
        pthread_cond_wait(&cond_out, &mut_out);
    total_allowed--;
    pthread_mutex_unlock(&mut_out);


    visitor_A();

    pthread_mutex_lock(&mut_out);
    total_allowed++;
    pthread_cond_signal(&cond_out);
    pthread_mutex_unlock(&mut_out);

    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    srand(time(NULL));
    to_enter = 1000;

    // CLI arguments
    if (argc > 2) {
        Na = atoi(argv[1]);
        Nb = atoi(argv[2]);
        if (argc > 3)
            to_enter = atoi(argv[3]);
    }
    else {
        Na = 63, Nb = 18;
    }

    // check if arguments are okay
    if (Na <= Nb) {
        printf("Capacity A <= capacity B\n");
        return 1;
    }
    else if (to_enter > 1000) {
        printf("Maximum supported number of threads: 1000");
        return 1;
    }


    int t = to_enter;

    total_allowed = Na + Nb - 1, freeA = Na, freeB = Nb;
    a = calloc(Na, sizeof(int));
    b = calloc(Nb, sizeof(int));


    pthread_t thread[1000];
    pthread_mutex_init(&mut, NULL);
    pthread_mutex_init(&mut_out, NULL);
    pthread_mutex_init(&mut_a, NULL);
    pthread_mutex_init(&mut_b, NULL);
    
    for (int i = 0; i < t; i++) 
        pthread_create(&thread[i], NULL, worker, NULL);
    

    for (int i = 0; i < t; i++) 
        pthread_join(thread[i], NULL);

    pthread_mutex_destroy(&mut);
    pthread_mutex_destroy(&mut_a);
    pthread_mutex_destroy(&mut_b);
    pthread_mutex_destroy(&mut_out);
    pthread_cond_destroy(&cond_a);
    pthread_cond_destroy(&cond_b);
    pthread_cond_destroy(&cond_out);
    free(b);
    free(a);

    return 0;
}
