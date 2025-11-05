#ifndef CLIENT_H
#define CLIENT_H

#include "server2.h"

typedef struct
{
   char nameadv[BUF_SIZE];
   int l[12];
   int score;
}Game;


typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   Game game;
   int etat; //0 rien, 1 cherche une partie, 2 joue et à son tour, 3 joue et pas son tour, 4 spectateur, 5 doit entrer un spectateur, 6 doit donner coup, 7 doit chatter et à tour, 8 chat et pas à son tour
}Client;

#endif /* guard */
