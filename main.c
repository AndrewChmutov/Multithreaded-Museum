#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define WIDTH 9

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


enum Decision{
    LEAVE,
    VISIT_B
} typedef Decision;


void visitor_A();

// pure output
void render_room(char room) {
    printf(" ");
    for (int i = 0; i < WIDTH; i++) {
        if (i == WIDTH / 2)
            printf("%c", room);
        else
            printf("_");
    }
    printf(" \n");

    int *arr;
    int s;
    if (room == 'A') {
        arr = a;
        s = Na;
    }
    else if (room == 'B') {
        arr = b;
        s = Nb;
    }
    else
        fprintf(stderr, "Error in rendering rooms: No such room\n");


    int n = ceil(1.0 * s / WIDTH);
    int k = 0;
    for (int i = 0; i < n; i++) {
        printf("|");
        for(int j = 0; j < WIDTH; j++) {
            if (k < s) 
                printf("%c", (arr[k] == 1 ? '#' : '.'));
            else
                printf("*");
            k++;
        }
        printf("|\n");
        
    }
}

// find place to fit in the room
int find_empty(char room) {
    int *arr;
    int s;
    if (room == 'A') {
        arr = a;
        s = Na;
    }
    else if (room == 'B') {
        arr = b;
        s = Nb;
    }
    
    for (int i = 0; i < s; i++) {
        if (arr[i] == 0)
            return i;
    }

    return -1;
}


// responsible for finding empty places,
// fitting, and exiting
int render(char room, int idx, int update) {

    if (room == 'A' && idx == -1 && update == 1)
        to_enter--;
    
    if (idx < 0)
        idx = find_empty(room);
    
    if (idx >= 0){
        if (room == 'A' && idx < Na)
            a[idx] ^= 1;
        else if (room == 'B' && idx < Nb)
            b[idx] ^= 1;
    }
    else
        fprintf(stderr, "Error: rendering\n");

    if (update) {
        usleep(100000);
        system("clear");
        render_room('B');
        render_room('A');

        printf("|");
        for (int i = 0; i < WIDTH; i++) {
            if (i == WIDTH / 2)
                printf(" ");
            else
                printf("_");
        }
        printf("|\n");
        printf("To enter: %d\n", to_enter);
    }

    return idx;
}


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

    free(b);
    free(a);

    return 0;
}
