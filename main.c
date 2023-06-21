#include <printf.h>
#include <stdlib.h>
#include "src/client/client.h"
#include "src/server/session.h"
#include "src/ship/ship.h"

int main() {
    int t, self_port, other_port, status, perdu;


    /*
     * Saisie du type d'instance.
     */
    printf("Héberger une partie ou rejoindre une partie ? [1/2] > ");
    scanf("%i", &t);

    if (t == 1) {
        self_port = SRV_PORT;
        other_port = CLT_PORT;
    } else {
        self_port = CLT_PORT;
        other_port = SRV_PORT;
    }

    /*
     * Initialisation du serveur
     */
    SESSION srv = start_session(self_port);

    /*
     * Création de la partie
     */
    // Génération de la grille
    BOARD board = init_board();

    printf("Placement de vos bateaux\n");
    printf("------------------------\n");

    board = place_ship(board, "porte avions", S_CARRIER, C_CARRIER);
    board = place_ship(board, "croiseur", S_BATTLESHIP, C_BATTLESHIP);
    board = place_ship(board, "contre-torpilleur", S_CRUISER, C_CRUISE);
    board = place_ship(board, "sous-marin", S_SUBMARINE, C_SUBMARINE);
    board = place_ship(board, "torpilleur", S_DESTROYER, C_DESTROYER);

    printf("\n==============================================================\n");
    printf(" Et c'est parti !");
    printf("\n==============================================================\n");

    status = S_HIT;

    // Server
    if (t == 1) {

        wait_handshake(srv);
        display_board(board.grid);
        while (status != S_LOST) {
            do {
                status = incoming_message(srv, &board);
            } while (status == S_HIT || status == S_SUNKEN);
            if (status != S_LOST) {
                do {
                    status = strike(other_port, &board);
                } while (status == S_HIT || status == S_SUNKEN);
            }
        }
    } else {
      // Client
        handshake(other_port);
        while (status != S_LOST) {
            do {
                status = strike(other_port, &board);
            } while (status == S_HIT || status == S_SUNKEN);

            if (status != S_LOST) {
                do {
                    status = incoming_message(srv, &board);
                } while (status == S_HIT || status == S_SUNKEN);
            }
        }
    }

    //End session
    end_session(srv, 0, "Bye.");

    return (EXIT_SUCCESS);
}