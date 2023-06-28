#include <sys/socket.h>
#include <printf.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "client.h"
#include "../point/point.h"
#include "../utils/console_colors.h"
#include "../server/session.h"

/**
 * Initialize the client
 *
 * @param port - Port the client will use
 * @return file desc to socket
 */
int init_client(int port) {
    int sock;
    struct sockaddr_in server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror(" ! erreur à la création du socket");
    }
    puts(" @ Socket en service");

    server.sin_addr.s_addr = inet_addr(CLT_ADDR);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    puts(" @ En attente...");
    while (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        // Waiting for the other player
    }

    puts(" @ Connexion etablie\n");

    return 0;
}

/**
 * Connect to the server
 *
 * @param server_port - Port to connect to
 * @param wait - Delay between tries
 * @return file desc to the server socket
 */
int connect_server(int server_port, int wait) {
    struct sockaddr_in server;
    int sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror(" ! erreur à la création du socket");
    }

    // socket setup (server)
    server.sin_addr.s_addr = inet_addr(CLT_ADDR);
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);

    if (wait == 0) {
        if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
            puts(" @ Le serveur n'est pas accessible.");
            return -1;
        }
    } else {
        while (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
            // Nothing. Infinite loop. Great.
        }
    }
    return sock;
}

/**
 * Send the strike to the other player
 *
 * @param other_port  - Port of the other player
 * @param board - The target bord
 * @return
 */
int strike(int other_port, BOARD *board) {
    char message[3 + 1], server_reply[3 + 1];
    int sock, server_response;
    POINT point;

    //Creation du socket
    sock = connect_server(other_port, 0);

    /*
     * "Interface graphique"
     */
    printf("\nGrille de l'adversaire :\n");
    display_board((*board).foe_grid);
    printf("\nA vous de jouer !\n");
    printf("Cible (ex: a1) : ");
    scanf("%s", message);
    point = str_to_point(message, 0);

    //Envoi de la chaine saisie par l'utilisateur
    if (send(sock, message, strlen(message), 0) < 0) {
        perror(" ! erreur lors de l'envoi");
        return -1;
    }

    //Réception du message de retour du serveur
    if (recv(sock, server_reply, sizeof(char) * 4, 0) < 0) {
        perror(" ! erreur lors de la reception de la reponse serveur");
        return -1;
    } else {
        server_response = strtol(server_reply, NULL, 10);
        printf("Résultat :\n");
        switch (server_response) {
            case S_MISS:
                printf(ANSI_COLOR_BLUE " => Eau...\n" ANSI_COLOR_RESET);
                (*board).foe_grid[point.y][point.x] = C_WATER_S;
                break;
            case S_HIT:
                printf(ANSI_COLOR_GREEN " => Boum...\n"ANSI_COLOR_RESET);
                (*board).foe_grid[point.y][point.x] = C_SHIP_S;
                break;
            case S_SUNKEN:
                printf(ANSI_COLOR_GREEN " => Glouglou !\n"ANSI_COLOR_RESET);
                (*board).foe_grid[point.y][point.x] = C_SHIP_S;
                break;
            case S_HIT_BIS:
                printf(ANSI_COLOR_BLUE " ! Cible déjà touchée ...\n" ANSI_COLOR_RESET);
                break;
            case S_LOST:
                printf(ANSI_COLOR_GREEN);
                printf("  Conglaturations !  You win ! \n");
                printf(ANSI_COLOR_RESET);
                break;
            default:
                printf(ANSI_COLOR_RED " ! Saisie non reconnue. (%i)\n" ANSI_COLOR_RESET, server_response);
        }

        return server_response;
    }

}

void handshake(int port) {
    int sock, msgSize = (sizeof(char) * 3);
    char message[2 + 1], server_reply[2 + 1];
    int srvV;

    sprintf(message, "%d", S_HANDSHAKE);

    printf("En attente de l'autre joueur...\n");

    sock = connect_server(port, 1);

    if (send(sock, message, msgSize, 0) < 0) {
        puts("Erreur lors de l'envoi de la poignée de main");
    }

    if (recv(sock, server_reply, msgSize, 0) < 0) {
        puts("Erreur lors de la reception de la reponse serveur");
    } else {
        srvV = strtol(server_reply, NULL, 10);
        if (srvV != S_HANDSHAKE) {
            printf("Réponse du serveur inconnue (%i)\n", srvV);
        } else {
            printf("Connecté à l'autre joueur\n");
        }
    }
    close(sock);
}

