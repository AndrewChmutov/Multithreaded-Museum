#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "multivisit.h"

// width of the output box
const int width = 9;

// output control
pthread_mutex_t mut;

// enter to rooms
pthread_mutex_t mut_a;
pthread_mutex_t mut_b;
pthread_cond_t cond_a;
pthread_cond_t cond_b;
int freeA, freeB;

// control over total amount of visitors
pthread_mutex_t mut_out;
pthread_cond_t cond_out;
int total_allowed;

// tracking the visitors
int Na, Nb;
int *a, *b;
int to_enter;


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

    // reserve one place for B visitors to leave in shortst time
    total_allowed = Na + Nb - 1, freeA = Na, freeB = Nb;

    // initialize arrays with 0 - nobody in
    a = calloc(Na, sizeof(int));
    b = calloc(Nb, sizeof(int));

    pthread_t thread[1000];

    // initialization of mutexes
    pthread_mutex_init(&mut, NULL);
    pthread_mutex_init(&mut_out, NULL);
    pthread_mutex_init(&mut_a, NULL);
    pthread_mutex_init(&mut_b, NULL);
    
    int t = to_enter;
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
