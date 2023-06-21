//
// Created by Ophélien DUPARC on 14/06/2023.
//

#include <stdio.h>
#include <libc.h>
#include "board.h"
#include "../ship/ship.h"
#include "../utils/console_colors.h"
#include "../point/point.h"


// Initializing the board
BOARD init_board() {
    BOARD board;
    int i, j;

    // Creating player army
    board.carrier = S_CARRIER;
    board.battleship = S_BATTLESHIP;
    board.cruiser = S_CRUISER;
    board.submarine = S_SUBMARINE;
    board.destroyer = S_DESTROYER;
    board.score = 0;

    // Filling the board with water (better if we want the ship to float, or sink)
    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            board.grid[i][j] = C_WATER;
            board.foe_grid[i][j] = C_WATER;
        }
    }


    return board;
}

/*
 * This code was optimised using valgrind
 *
 * 0. border
 * 1. Water
 * 2. Water Striken
 * 3. A ship
 * 4. A striken ship
 */
const char line[] = "  +===+===+===+===+===+===+===+===+===+===+",
        WATER[] = "   |",
        WATER_S[] = ANSI_COLOR_BLUE " X " ANSI_COLOR_RESET "|",
        SHIP[] = ANSI_COLOR_GREEN "***" ANSI_COLOR_RESET "|",
        SHIP_S[] = ANSI_COLOR_RED "XXX" ANSI_COLOR_RESET "|";


void display_board(int board[BOARD_SIZE][BOARD_SIZE]) {
    int i, j;

    puts("\n  | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10|");
    puts(line);
    for (i = 0; i < BOARD_SIZE; i++) {
        printf("%c |", i + 'A');
        for (j = 0; j < BOARD_SIZE; j++) {
            if ((int) board[i][j] == C_WATER) { // Water
                printf("%d", C_WATER);
            } else if (board[i][j] == C_WATER_S) { // Water Stiken
                printf("%d", C_WATER_S);
            } else if (// Ship
                    board[i][j] == C_CARRIER ||
                    board[i][j] == C_BATTLESHIP ||
                    board[i][j] == C_CRUISE ||
                    board[i][j] == C_SUBMARINE ||
                    board[i][j] == C_DESTROYER
                    ) {
                printf("%s", SHIP);
            } else {
                printf("%s", SHIP_S);
            }
        }
        printf("\n");
        puts(line);
    }
}


int get_board_score(BOARD board) {
    return board.carrier + board.battleship + board.cruiser + board.submarine + board.destroyer;
}

BOARD place_ship(BOARD board, char nom[], int size, int val) {
    int done, error, i;
    POINT point;
    char pos[4 + 1], // Coordonnées
    orientation[10 + 1],
            reponse;

    display_board(board.grid);
    // IF YOU WANT TO DEBUG/TEST THE GAME,
    // COMMENT FROM HERE
    printf("Pour placer un bateau, donnez l'adresse de la case de destination, puis\n"
           "son orientation (h/v). Les batiments seront positionnés sur la droite de\n"
           "la case donnée pour les placements horizontaux, et vers le base pour les \n"
           "placements verticaux. Exemple : a10v\n\n");
    printf("Veuillez placer le %s (%i cases)\n\n", nom, size);
    do {
        // Re-init vars
        error = 0;
        done = 0;
        strcpy(orientation, "horizontal");

        // Ask for coordinates
        printf("Position : ");
        scanf("%s", pos);

        point = str_to_point(pos, 1);

        //		printf("x: %i, y: %i, o: %point\n", point.x, point.y, point.d);

        if (point.x < 0 || point.y < 0 || point.x > BOARD_SIZE || point.y > BOARD_SIZE) {
            puts(" > Mauvaises coordonnées...");
            error = 1;
        } else if (point.state == 'v') {// Vérification placement des bateaux
            strcpy(orientation, "vertical");
            // Sortie de carte
            if (point.y + size > BOARD_SIZE) {
                printf(" > Vous ne pouvez pas placer votre bateau ici. Il sortirait de la carte...(y=%i)\n", point.y);
                error = 1;
            } else {
                // Chevauchements
                for (i = point.y; i < point.y + size; i++) {
                    if (board.grid[i][point.x] != C_WATER) {
                        puts(" > Il y a déjà un bateau ici...");
                        error = 1;

                        break;
                    }
                }
            }
        } else if (point.x + size > BOARD_SIZE) {
            printf(" > Vous ne pouvez pas placer votre bateau ici. Il sortirait de la carte...(x=%i)\n", point.x);
            error = 1;
        } else {
            // Chevauchements
            for (i = point.x; i < point.x + size; i++) {
                if (board.grid[point.y][i] != C_WATER) {
                    puts(" > Il y a déjà un bateau ici...");
                    error = 1;

                    break;
                }
            }
        }

        if (error == 0) {
            getchar();
            printf("Placement %s en %point:%i. Est-ce correct ? [o/N] ", orientation, point.y + 'a', point.x + 1);
            reponse = getchar();
            if (reponse == 'o' || reponse == 'O') {
                done = 1;
            }
        }
    } while (done == 0);

    // Place ship on grid
    if (point.state == 'v') {
        for (i = point.y; i < point.y + size; i++) {
            board.grid[i][point.x] = val;
        }
    } else {
        for (i = point.x; i < point.x + size; i++) {
            board.grid[point.y][i] = val;
        }
    }
    return board;
}
