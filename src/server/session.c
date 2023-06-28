
#include <unistd.h>
#include <printf.h>
#include <stdlib.h>
#include "session.h"
#include "../point/point.h"
#include "../utils/console_colors.h"


/*
 * Create a new session using a port
 */
SESSION start_session(int port) {
    SESSION session;

    // Trying to open socket
    session.socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (session.socket_desc == -1) {
        end_session(session, 1, "Impossible de créer le socket.");
    }

    // Setting up server
    session.server.sin_family = AF_INET;
    session.server.sin_addr.s_addr = INADDR_ANY;
    session.server.sin_port = htons(port); // Convert port to network format

    // Binding the session to the socket
    if (bind(session.socket_desc, (struct sockaddr *) &session.server, sizeof(session.server)) < 0) {
        end_session(session, 1, "Erreur de la mise en place de l'écoute.");
    }

    // listening incoming connections with a buffer of three
    listen(session.socket_desc, 3);

    printf(" @ Serveur en place (port %i). \n En attente de client..\n", port);
    session.c = sizeof(struct sockaddr_in);

    return session;
}


int incoming_message(SESSION session, BOARD *board) {
    char server_response[20];
    int content, status = 0, pv = -1;
    POINT hit;

    // Incoming client accept
    session.client_sock = accept(session.socket_desc, (struct sockaddr *) &session.client, (socklen_t *) &session.c);
    if (session.client_sock < 0) {
        perror(" ! Connexion entrante refusee");
        return 1;
    }

    printf("En attente du hit de l'adversaire..\n");

    // Reading message
    while ((session.read_size = recv(session.client_sock, session.client_message, 100, 0)) > 0) {
        printf(ANSI_COLOR_BLUE " => %s\n\n" ANSI_COLOR_RESET, session.client_message);


        hit = str_to_point(session.client_message, 0);
        content = (*board).grid[hit.y][hit.x];

        if (content == C_WATER || content == C_WATER_S) {
            sprintf(server_response, "%d", S_MISS);
            (*board).grid[hit.y][hit.x] = C_WATER_S;
            status = S_MISS;
        } else {
            status = S_HIT;
            sprintf(server_response, "%d", S_HIT);
            switch (content) {
                case C_CARRIER:
                    (*board).grid[hit.y][hit.x] = C_CARRIER_S;
                    (*board).carrier--;
                    pv = (*board).carrier;
                    break;
                case C_BATTLESHIP:
                    (*board).grid[hit.y][hit.x] = C_BATTLESHIP_S;
                    (*board).battleship--;
                    pv = (*board).battleship;
                    break;
                case C_CRUISE:
                    (*board).grid[hit.y][hit.x] = C_CRUISER_S;
                    (*board).cruiser--;
                    pv = (*board).cruiser;
                    break;
                case C_SUBMARINE:
                    (*board).grid[hit.y][hit.x] = C_SUBMARINE_S;
                    (*board).submarine--;
                    pv = (*board).submarine;
                    break;
                case C_DESTROYER:
                    (*board).grid[hit.y][hit.x] = C_DESTROYER_S;
                    (*board).destroyer--;
                    pv = (*board).destroyer;
                    break;
                default: // Bateau déjà touché
                    sprintf(server_response, "%d", S_HIT_BIS);
                    status = S_MISS;
            }
        }

        if (pv == 0) {
            status = S_HIT;
            sprintf(server_response, "%d", S_SUNKEN);
        }

        printf("Résultat :\n");
        display_board((*board).grid);
        printf(ANSI_COLOR_BLUE "\nIl vous reste %i points..\n" ANSI_COLOR_RESET, get_board_score(*board));
        if (get_board_score(*board) == 0) {
            printf(ANSI_COLOR_RED"..Vous avez perdu.\n"ANSI_COLOR_RESET);
            sprintf(server_response, "%d", S_LOST);
        }
        // Envoi d'une réponse au client
        write(session.client_sock, server_response, sizeof(char) * 4);

        if (get_board_score(*board) == 0) {
            return S_LOST;
        }

        break;
    }

    if (session.read_size == 0) {
        puts(" ! Déconnexion du client");
        fflush(stdout);
    } else if (session.read_size == -1) {
        perror(" ! Erreur lors de la réception");
    }

    return status;
}


int wait_handshake(SESSION session) {
    int content;
    char resp[2 + 1];
    sprintf(resp, "%d", S_HANDSHAKE);

    printf(" # En attente de l'autre joueur...\n");
    // Acceptation de la connexion d'un client
    session.client_sock = accept(session.socket_desc, (struct sockaddr *) &session.client, (socklen_t *) &session.c);
    if (session.client_sock < 0) {
        perror(" ! Connexion entrante refusee");
        return 1;
    }

    while ((session.read_size = recv(session.client_sock, session.client_message, 100, 0)) > 0) {
        // Traitement du retours
        content = strtol(session.client_message, NULL, 10);
        if (content == S_HANDSHAKE) {
            write(session.client_sock, resp, sizeof(int));
            printf(" # Connecté à l'autre joueur...\n");
        }
    }

    return 0;
}

/*
 * End the current sesion and close sockets
 */
void end_session(SESSION session, int code, char message[50]) {
    // Closes sockets
    close(session.socket_desc);
    close(session.client_sock);

    // Display message
    printf("\n%s\n", message);

    // Exit
    exit(code);
}