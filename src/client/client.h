//
// Created by Ophélien DUPARC on 14/06/2023.
//

#ifndef BATTLESHIP_CLIENT_H
#define BATTLESHIP_CLIENT_H

#include "../board/board.h"

#define CLT_PORT 8889
#define CLT_ADDR "127.0.0.1"

int init_client(int port);

int connect_server(int server_port, int wait);

int strike(int other_port, BOARD *board);

void handshake(int port);

#endif //BATTLESHIP_CLIENT_H
