#include <printf.h>
#include <stdlib.h>
#include "src/client/client.h"
#include "src/server/session.h"
#include "src/ship/ship.h"

int main() {
    int t, self_port, other_port, state, lost;


    // Game type
    printf(" # Serveur (1) ou client (2) ? [1/2] > ");
    scanf("%i", &t);

    if (t == 1) {
        self_port = SRV_PORT;
        other_port = CLT_PORT;
    } else {
        self_port = CLT_PORT;
        other_port = SRV_PORT;
    }


    // Init session
    SESSION srv = start_session(self_port);

    BOARD board = init_board();

    printf("Placement de vos bateaux\n");

    board = place_ship(board, "porte avions", S_CARRIER, C_CARRIER);
    board = place_ship(board, "croiseur", S_BATTLESHIP, C_BATTLESHIP);
    board = place_ship(board, "contre-torpilleur", S_CRUISER, C_CRUISE);
    board = place_ship(board, "sous-marin", S_SUBMARINE, C_SUBMARINE);
    board = place_ship(board, "torpilleur", S_DESTROYER, C_DESTROYER);

    printf(" Début de la partie\n");

    state = S_HIT;

    // Server
    if (t == 1) {

        wait_handshake(srv);
        display_board(board.grid);
        while (state != S_LOST) {
            do {
                state = incoming_message(srv, &board);
            } while (state == S_HIT || state == S_SUNKEN);
            if (state != S_LOST) {
                do {
                    state = strike(other_port, &board);
                } while (state == S_HIT || state == S_SUNKEN);
            }
        }
    } else {
      // Client
        handshake(other_port);
        while (state != S_LOST) {
            do {
                state = strike(other_port, &board);
            } while (state == S_HIT || state == S_SUNKEN);

            if (state != S_LOST) {
                do {
                    state = incoming_message(srv, &board);
                } while (state == S_HIT || state == S_SUNKEN);
            }
        }
    }

    //End session
    end_session(srv, 0, "Bye.");

    return (EXIT_SUCCESS);
}