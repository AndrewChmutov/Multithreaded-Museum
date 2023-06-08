#pragma once

extern int width;
extern int *a, *b;
extern int Na, Nb;
extern int to_enter;

void render_room(char room);
int find_empty(char room);
int render(char room, int idx, int update);
