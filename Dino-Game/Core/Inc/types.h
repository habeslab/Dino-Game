#ifndef TYPES_H
#define TYPES_H



#define MAX_OBSTACLES 10
#include <stdbool.h>



typedef struct {
    int position_row;
    int position_col;
    bool jump;
    bool down;
} Dino;

typedef struct {
    int position_row[MAX_OBSTACLES];
    int position_col[MAX_OBSTACLES];
} Obstacles;


#endif
