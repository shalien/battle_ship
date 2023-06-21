//
// Created by Ophélien DUPARC on 14/06/2023.
//

#ifndef BATTLESHIP_BOARD_H
#define BATTLESHIP_BOARD_H

#define BOARD_SIZE 10

// All possibles board cells values (for display)
#define C_WATER 0 // Water
#define C_WATER_S 1 // Water hit
#define C_SHIP_S 2 // Hit ship
#define C_CARRIER 10 // Carrier
#define C_CARRIER_S 11 // Carrier hit
#define C_BATTLESHIP 20 // Battleship
#define C_BATTLESHIP_S 11 // Battleship hit
#define C_CRUISE 30 // Cruiser
#define C_CRUISER_S 31 // Cruiser hit
#define C_SUBMARINE 40 // Submarine
#define C_SUBMARINE_S 41 // Submarine hit
#define C_DESTROYER 50 // Destroyer
#define C_DESTROYER_T 51 // Destroyer hit

/*
 * This struct represents the board game of both players
 */
typedef struct BOARD {
    //   5        4         3                  3         2
    int carrier, battleship, cruiser, submarine, destroyer;
    int score;
    int grid[BOARD_SIZE][BOARD_SIZE];
    int foe_grid[BOARD_SIZE][BOARD_SIZE];
} BOARD;


BOARD init_board();

int get_board_score(BOARD board);

void display_board(int board[BOARD_SIZE][BOARD_SIZE]);

BOARD place_ship(BOARD board, char nom[], int size, int val);

#endif //BATTLESHIP_BOARD_H
