//
// Created by Ophélien DUPARC on 14/06/2023.
//

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


int init_client(int port) {
    int sock;
    struct sockaddr_in server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("erreur à la création du socket");
    }
    puts("Socket en service");

    server.sin_addr.s_addr = inet_addr(CLT_ADDR);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    puts("En attente de l'autre joueur...");
    while (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
    }

    puts("Connexion etablie\n");

    return 0;
}

int connect_server(int server_port, int wait) {
    struct sockaddr_in server;
    int sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("erreur à la création du socket");
    }

    // Connexion au serveur
    server.sin_addr.s_addr = inet_addr(CLT_ADDR); //définition de l'adresse IP du serveur
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port); //définition du port d'écoute du serveur

    if (wait == 0) {
        if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
            puts("Le serveur n'est pas accessible.");
            return -1;
        }
    } else {
        while (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
            // Nothing. Infinite loop. Great.
        }
    }
    return sock;
}


int strike(int other_port, BOARD *board) {
    char message[3 + 1], server_reply[3 + 1];
    int sock, srvR;
    POINT point;

    //Creation du socket
    sock = connect_server(other_port, 0);

    /*
     * "Interface graphique"
     */
    printf("\nGrille de l'adversaire :\n");
    printf("------------------------\n");
    display_board((*board).foe_grid);
    printf("\nA vous de jouer !\n");
    printf("Entrez les coordonnées à frapper (ex: a1) : ");
    scanf("%s", message);
    point = str_to_point(message, 0);

    //Envoi de la chaine saisie par l'utilisateur
    if (send(sock, message, strlen(message), 0) < 0) {
        puts("erreur lors de l'envoi");
        return -1;
    }

    //Réception du message de retour du serveur
    if (recv(sock, server_reply, sizeof(char) * 4, 0) < 0) {
        puts("erreur lors de la reception de la reponse serveur");
        return -1;
    } else {
        // Conversion réponse
        srvR = strtol(server_reply, NULL, 10);
        printf("Résultat :\n");
        printf("----------\n");
        //		printf("[%i]", srvR);
        switch (srvR) {
            case S_MISS:
                printf(ANSI_COLOR_BLUE " > Dans l'eau...\n" ANSI_COLOR_RESET);
                (*board).foe_grid[point.y][point.x] = C_WATER_S;
                break;
            case S_HIT:
                printf(ANSI_COLOR_GREEN " > Touché...\n"ANSI_COLOR_RESET);
                (*board).foe_grid[point.y][point.x] = C_SHIP_S;
                break;
            case S_SUNKEN:
                printf(ANSI_COLOR_GREEN " > Coulé !\n"ANSI_COLOR_RESET);
                (*board).foe_grid[point.y][point.x] = C_SHIP_S;
                break;
            case S_HIT_BIS:
                printf(ANSI_COLOR_BLUE " ! Vous avez déjà touché ici...\n" ANSI_COLOR_RESET);
                break;
            case S_LOST:
                printf(ANSI_COLOR_GREEN);
                printf("=============================\n\n");
                printf("  Vous avez gagné ! Bravo !\n");
                printf("=============================\n\n");
                printf(ANSI_COLOR_RESET);
                break;
            default:
                printf(ANSI_COLOR_RED " ! Réponse incompréhensible. (%i)\n" ANSI_COLOR_RESET, srvR);
        }

        return srvR;
    }

}

void handshake(int port) {
    int sock, msgSize = (sizeof(char) * 3);
    char message[2 + 1], server_reply[2 + 1];
    int srvV;

    sprintf(message, "%d", S_HANDSHAKE);

    printf("En attente de l'autre joueur...\n");

    sock = connect_server(port, 1);
    //Envoi du code de poignée de main

    if (send(sock, message, msgSize, 0) < 0) {
        puts("Erreur lors de l'envoi de la poignée de main");
    }

    //Réception du message de retour du serveur
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

