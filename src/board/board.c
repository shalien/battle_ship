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
const char
        WATER[] = " ≈≈≈",
        WATER_S[] = ANSI_COLOR_BLUE " X " ANSI_COLOR_RESET " ",
        SHIP[] = ANSI_COLOR_GREEN "±±±" ANSI_COLOR_RESET " ",
        SHIP_S[] = ANSI_COLOR_RED "﬩﬩﬩" ANSI_COLOR_RESET " ";


void display_board(int board[BOARD_SIZE][BOARD_SIZE]) {
    int i, j;

    puts("\n    1   2   3   4   5   6   7   8   9   10 ");
    for (i = 0; i < BOARD_SIZE; i++) {
        printf("%c ", i + 'A');
        for (j = 0; j < BOARD_SIZE; j++) {
            if ((int) board[i][j] == C_WATER) { // Water
                printf("%s", WATER);
            } else if (board[i][j] == C_WATER_S) { // Water Stiken
                printf("%s", WATER_S);
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
    }
}


int get_board_score(BOARD board) {
    return board.carrier + board.battleship + board.cruiser + board.submarine + board.destroyer;
}

BOARD place_ship(BOARD board, char name[], int size, int val) {
    int done, error, i;
    POINT point;
    char pos[4+ 1], //  + 1 pour la touche entrée
     heading[1 + 1],
     result[5] = {0},
    orientation[10 + 1],
            response;

    display_board(board.grid);
    printf("Veuillez placer le %s (%i cases)\n\n", name, size);
    do {
        // Re-init vars
        error = 0;
        done = 0;
        strcpy(orientation, "horizontal");

        // Ask for coordinates
        printf("Position (ex: B10) : ");
        scanf("%s", pos);

        printf("Orientation (h/v) : ");
        scanf("%s", heading);

        char trimmedPos[4];
        strlcpy(trimmedPos, pos, sizeof(trimmedPos));

        char trimmedHeading[2];
        strlcpy(trimmedHeading, heading, sizeof(trimmedHeading));

        strlcat(result, trimmedPos, 4);
        strcat(result, trimmedHeading);

        point = str_to_point(result, 1);

        if (point.x < 0 || point.y < 0 || point.x > BOARD_SIZE || point.y > BOARD_SIZE) {
            puts(" # Position incorrecte...");
            error = 1;
        } else if (point.state == 'v') {
            strcpy(orientation, "vertical");
            // Sortie de carte
            if (point.y + size > BOARD_SIZE) {
                printf(" # Position incorrecte. Dépassement de la zone de jeu...(y=%i)\n", point.y);
                error = 1;
            } else {
                // Chevauchements
                for (i = point.y; i < point.y + size; i++) {
                    if (board.grid[i][point.x] != C_WATER) {
                        puts(" # Position incorrecte, Case occupée...");
                        error = 1;

                        break;
                    }
                }
            }
        } else if (point.x + size > BOARD_SIZE) {
            printf(" # Position incorrecte. Dépassement de la zone de jeu...(x=%i)\n", point.x);
            error = 1;
        } else {
            // Chevauchements
            for (i = point.x; i < point.x + size; i++) {
                if (board.grid[point.y][i] != C_WATER) {
                    puts(" # Position incorrecte, Case occupée...");
                    error = 1;

                    break;
                }
            }
        }

        if (error == 0) {
            getchar();
            printf(" # Position %s en %c:%i. \n # Est-ce correct ? [o/N] ", orientation, point.y + 'a', point.x + 1);
            response = getchar();
            if (response == 'o' || response == 'O') {
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
