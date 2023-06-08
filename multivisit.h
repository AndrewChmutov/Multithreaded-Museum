#pragma once
#include <pthread.h>

extern pthread_mutex_t mut;
extern pthread_mutex_t mut_a;
extern pthread_mutex_t mut_b;
extern pthread_mutex_t mut_out;
extern pthread_cond_t cond_a;
extern pthread_cond_t cond_b;
extern pthread_cond_t cond_out;
extern int freeA, freeB, total_allowed;


void visitor_A();
void visitor_B();
void *worker(void *arg);
