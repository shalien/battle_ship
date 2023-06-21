//
// Created by Ophélien DUPARC on 14/06/2023.
//

#ifndef BATTLESHIP_SESSION_H
#define BATTLESHIP_SESSION_H


#include <netinet/in.h>
#include "../board/board.h"

#define SRV_PORT 7890

#define S_LOST -1 // Partie perdue
#define S_HIT 1 // Bateau touché
#define S_MISS 2 // Bateau manqué
#define S_HIT_BIS 3 // Bateau touché
#define S_SUNKEN 4 //Bateau coulé.
#define S_HANDSHAKE 10 // Code de poignée de main


typedef struct SESSION {
    int socket_desc;
    int client_sock;
    int c;
    int read_size;
    struct sockaddr_in server;
    struct sockaddr_in client;
    char client_message[2000];

} SESSION;

SESSION start_session(int port);

void end_session(SESSION session, int code, char message[50]);

int wait_handshake(SESSION session);

int incoming_message(SESSION session, BOARD *board);

#endif //BATTLESHIP_SESSION_H
