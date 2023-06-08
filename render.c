#include "render.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>


// just render the room
void render_room(char room) {
    // set up variables and links for a room
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

    // upper line
    printf(" ");
    for (int i = 0; i < width; i++) {
        if (i == width / 2)
            printf("%c", room);
        else
            printf("_");
    }
    printf(" \n");

    // contents of a box
    int n = ceil(1.0 * s / width);
    int k = 0;
    for (int i = 0; i < n; i++) {
        printf("|");

        for(int j = 0; j < width; j++) {
            // # - busy
            // . - empty
            // * - padding, not empty, not busy
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
    // set up variables and links for a room
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

    // if no room found
    // in theory should not be executed
    return -1;
}


// responsible for finding empty places,
// fitting, and exiting (on output)
int render(char room, int idx, int update) {

    // counter of people in the queue
    if (room == 'A' && idx == -1 && update == 1)
        to_enter--;
    
    if (idx < 0)
        idx = find_empty(room);
    
    // switch the status of a cell
    if (idx >= 0){
        if (room == 'A' && idx < Na)
            a[idx] ^= 1;
        else if (room == 'B' && idx < Nb)
            b[idx] ^= 1;
    }
    // if there is no place to enter, though
    // logic of the program implies that it entered
    // should not be executed
    else
        fprintf(stderr, "Error: rendering\n");

    if (update) {
        usleep(100000);
        
        // render
        system("clear");
        render_room('B');
        render_room('A');

        // last line
        printf("|");
        for (int i = 0; i < width; i++) {
            if (i == width / 2)
                printf(" ");
            else
                printf("_");
        }
        printf("|\n");

        // counter for a queue
        printf("To enter: %d\n", to_enter);
    }

    return idx;
}
