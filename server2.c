#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server2.h"
#include "client2.h"

static void init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}


int affichage(int l[12], char *buffer, size_t taille_buffer)
{
    int i;
    int offset = 0;

    // Première ligne
    for (i = 0; i < 6 && offset < taille_buffer; i++) {
        offset += snprintf(buffer + offset, taille_buffer - offset, "%d   ", l[i]);
    }
    offset += snprintf(buffer + offset, taille_buffer - offset, "\n");

    // Deuxième ligne
    for (i = 11; i > 5 && offset < taille_buffer; i--) {
        offset += snprintf(buffer + offset, taille_buffer - offset, "%d   ", l[i]);
    }
    offset += snprintf(buffer + offset, taille_buffer - offset, "\n");

    return 0;
}

static void app(void)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   /* the index for the array */
   int actual = 0;
   int max = sock;
   /* an array for all clients */
   Client clients[MAX_CLIENTS];

   fd_set rdfs;

   while(1)
   {
      int i = 0;
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for(i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         /* stop process when type on keyboard */
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         /* new client */
         SOCKADDR_IN csin = { 0 };
         size_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if(csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* after connecting the client sends its name */
         if(read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }
         char name1[20];
         strcpy(name1, buffer);
         printf("%s\n", buffer);
         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;
         
         FD_SET(csock, &rdfs);

         Client c = { csock };
         strncpy(c.name, buffer, BUF_SIZE - 1);
         clients[actual] = c;
         actual++;
         if (actual < 2)
         {
            strcpy(buffer, "Il manue un joueur pour commencer");
            send_message_to_all(clients, actual, buffer, 0);
         }
         
         if (actual == 2)
         {

            strcpy(buffer, "2 joueurs sont connectés la partie va commencer !");
            send_message_to_all(clients, actual, buffer, 0);
            int l[12];
            int joueur1=0, joueur2=0, i, nb;
            for(i=0; i<12; i++)
            {
               l[i]=4;
            }


            while (joueur1 < 25 && joueur2<25)
            {
            int tour = 0, choix = 0, bon =1;
            //char buffer2[150];
            if (bon)
            {
               sprintf(buffer, "\n\n NOUVEAU TOUR \n\nPoint du joueur1: %d\nPoint du joueur2: %d\n", joueur1, joueur2);

            send_message_to_all(clients, actual, buffer, 0);
            
            affichage(l, buffer, BUF_SIZE);
            send_message_to_all(clients, actual, buffer, 0);

            strcpy(buffer, "Joueur1: choisissez la case à repartir (1 à 6)\n");
            send_message_to_all_clients(clients, clients[1], actual, buffer, 0);

            }
            bon = 0;
            




            i = 0;
            FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for(i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         /* stop process when type on keyboard */
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         /* new client */
         SOCKADDR_IN csin = { 0 };
         size_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if(csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* after connecting the client sends its name */
         if(read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }
         char name1[20];
         strcpy(name1, buffer);
         printf("%s\n", buffer);
         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;
         
         FD_SET(csock, &rdfs);

         Client c = { csock };
         strncpy(c.name, buffer, BUF_SIZE - 1);
         clients[actual] = c;
         actual++;
         
                    

         }

      else
      {
         int i = 0;
         if (actual > 1)
         {
            for(i = 0; i < actual; i++)
            {
               /* a client is talking */
               if (i==tour)
               {
                  if(FD_ISSET(clients[i].sock, &rdfs))
                  {
                     Client client = clients[i];
                     int c = read_client(clients[i].sock, buffer);
                     /* client disconnected */
                     if(c == 0)
                     {
                        closesocket(clients[i].sock);
                        remove_client(clients, i, &actual);
                        strncpy(buffer, client.name, BUF_SIZE - 1);
                        strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                        send_message_to_all_clients(clients, client, actual, buffer, 1);
                     }
                     else
                     {
                        choix = atoi(buffer);
                        printf("%d\n", choix);
                        if(choix<1 || choix>6)
                        {  
                           strcpy(buffer, "Nombre invalide veuillez reessayer\n");   
                        }   
                        send_message_to_all_clients(clients, clients[1], actual, buffer, 0); //A MODIFER
                     }
                     break;
                  }
               
               }
               else
               {
                  if(FD_ISSET(clients[i].sock, &rdfs))
                  {
                     Client client = clients[i];
                     int c = read_client(clients[i].sock, buffer);
                     /* client disconnected */
                     if(c == 0)
                     {
                        closesocket(clients[i].sock);
                        remove_client(clients, i, &actual);
                        strncpy(buffer, client.name, BUF_SIZE - 1);
                        strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                        send_message_to_all_clients(clients, client, actual, buffer, 1);
                     }
                     else
                     {
                        send_message_to_all_clients(clients, client, actual, buffer, 0);
                     }
                     break;
                  }
               }
            }
         }
   }








            
                  }
           
             }       

         }

      else
      {
         int i = 0;
         if (actual > 1)
         {
            for(i = 0; i < actual; i++)
            {
               /* a client is talking */
               if(FD_ISSET(clients[i].sock, &rdfs))
               {
                  Client client = clients[i];
                  int c = read_client(clients[i].sock, buffer);
                  /* client disconnected */
                  if(c == 0)
                  {
                     closesocket(clients[i].sock);
                     remove_client(clients, i, &actual);
                     strncpy(buffer, client.name, BUF_SIZE - 1);
                     strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                     send_message_to_all_clients(clients, client, actual, buffer, 1);
                  }
                  else
                  {
                     send_message_to_all_clients(clients, client, actual, buffer, 0);
                  }
                  break;
               }
            }
         }
   }
   }

   clear_clients(clients, actual);
   end_connection(sock);
}

static void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_client(Client *clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual)--;
}

static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(sender.sock != clients[i].sock)
      {
         if(from_server == 0)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
      }
   }
}

/**
 * Envoie un message à un client spécifique.
 * @param clients Tableau de tous les clients connectés
 * @param actual Nombre actuel de clients connectés
 * @param target Nom du client à qui envoyer le message
 * @param buffer Message à envoyer
 * @param from_server 0 = message venant d'un client, 1 = message venant du serveur
 */
void send_to_specific_client(Client *clients, int actual, const char *target, const char *buffer, char from_server)
{
    char message[BUF_SIZE];
    message[0] = '\0';

    // Cherche le client cible
    for (int i = 0; i < actual; i++)
    {
        if (strcmp(clients[i].name, target) == 0)
        {
            // Prépare le message
            if (from_server == 0)
            {
                snprintf(message, BUF_SIZE, "%s : %s", "serveur", buffer);
            }
            else
            {
                strncpy(message, buffer, BUF_SIZE - 1);
            }

            // Envoie au client cible
            write_client(clients[i].sock, message);
            return;
        }
    }

    // Si le client n’existe pas
    printf("⚠️ Client '%s' introuvable.\n", target);
}



void send_message_to_all(Client *clients, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
         if(from_server == 0)
         {
            strncpy(message, "serveur", BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
   }
}



static int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

static void write_client(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}
