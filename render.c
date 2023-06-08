#include "render.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

extern int width;
extern int *a, *b;
extern int Na, Nb;
extern int to_enter;

void render_room(char room) {
    printf(" ");
    for (int i = 0; i < width; i++) {
        if (i == width / 2)
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


    int n = ceil(1.0 * s / width);
    int k = 0;
    for (int i = 0; i < n; i++) {
        printf("|");
        for(int j = 0; j < width; j++) {
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
        for (int i = 0; i < width; i++) {
            if (i == width / 2)
                printf(" ");
            else
                printf("_");
        }
        printf("|\n");
        printf("To enter: %d\n", to_enter);
    }

    return idx;
}

