#ifndef TYPES_H
#define TYPES_H

#define MAX_OBSTACLES 300  // Maximum number of obstacles that can be present in the game
#define MIN_OBSTACLE_DISTANCE 3


#include <stdbool.h> // Include standard boolean type definitions

// Structure to represent the player character (Dino) in the game
typedef struct {
    int position_row; // Row position of the Dino on the LCD display
    int position_col; // Column position of the Dino on the LCD display
    bool jump;        // Flag indicating whether the Dino is currently jumping
    bool down;        // Flag indicating whether the Dino is currently ducking
} Dino;

// Structure to represent obstacles in the game
typedef struct {
    int position_row[MAX_OBSTACLES]; // Array to store the row positions of obstacles
    int position_col[MAX_OBSTACLES]; // Array to store the column positions of obstacles
} Obstacles;

#endif

