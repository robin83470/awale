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
         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;
         
         FD_SET(csock, &rdfs);

         Client c = { csock };
         c.game.etat = 0;
         strncpy(c.name, buffer, BUF_SIZE - 1);
         clients[actual] = c;
         actual++;
         
         strcpy(buffer, "\n\nTu es connecté au serveur, que veux tu faire, 1 pour joeur, 2 pour être spectateur d'une partie\n\n");
         send_message_to_clients(clients, c, actual, buffer);      

         
      }
      else
      {
         int i = 0;
         int etat, nb;
            for(i = 0; i < actual; i++)
            {
               /* a client is talking */
               if(FD_ISSET(clients[i].sock, &rdfs))
               {
                  Client client = clients[i];
                  int c = read_client(clients[i].sock, buffer);
                  etat = client.game.etat;
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
                     printf("%s\n", buffer);
                     nb = atoi(buffer);
                     if (etat==0)
                     {
                        if(nb == 1)
                        {
                           clients[i].game.etat = 1;
                           strcpy(buffer, "\n\nNous cherchons une partie, 0 pour annuler\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                           find_game(clients, actual, i);
                   

                        }
                        else if(nb == 2)
                        {
                           clients[i].game.etat = 5;
                           strcpy(buffer, "\n\nQui veut tu observer?\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer); 
                        }
                     }

                     else if (etat==1)
                     {
                        if(nb == 0)
                        {
                           clients[i].game.etat = 0;
                           strcpy(buffer, "\n\nNous allons la recherche\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                   

                        }
                        else
                        {
                           strcpy(buffer, "\n\nMessage non compris\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer); 
                        }
                     }

                     else if (etat==2)
                     {
                        int choix = nb;
                        if(choix<1 || choix>6 || clients[i].game.l[choix-1]==0 || !coup_valide(clients[i].game.l, choix, 1, joueur1, joueur2))
                        {
                              if (choix<1 || choix>6 || clients[i].game.l[choix-1]==0)
                                 printf("Nombre invalide, veuillez reessayer\n");
                              else if (!coup_valide(clients[i].game.l, choix, 1, joueur1, joueur2))
                                 printf("Ce coup affame l’adversaire, choisissez une autre case\n");
                        }
                        else
                        { 
                           strcpy(buffer, "\n\nMessage non compris\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer); 
                        }

                        //jouer_coup(l, choix, 1, &joueur1, &joueur2);
                                    

                        
                        
                     }
                     
                  }
                  break;
               }
            }
         
      }
   }

   clear_clients(clients, actual);
   end_connection(sock);
}

// Vérifie si un côté du plateau a encore des graines
int graines_restantes(int l[12], int debut, int fin)
{
    int somme = 0;
    for(int i = debut; i < fin; i++)
        somme += l[i];
    return somme;
}


void copier_plateau(int src[12], int dest[12])
{
    for(int i=0; i<12; i++)
        dest[i] = src[i];
}



// Joue un coup (sans vérification), sur un plateau donné, pour un joueur donné
void jouer_coup(int l[12], int choix, int joueur, int *score1, int *score2)
{
    int i, j, nb;

    nb = l[choix-1];
    l[choix-1] = 0;

    for(i = 1; i < nb+1; i++)
    {
        if((choix-1+i) > 11)
        {
            if(((choix-1+i - 12) == (choix-1)))
                continue;
            l[(choix-1+i) % 12] += 1;
        }
        else
        {
            if(((choix-1+i) % 12) == (choix-1))
                continue;
            l[(choix-1+i) % 12] += 1;
        }
    }

    // Captures
    for(i = 0; i < nb; i++)
    {
        j = (choix + nb - i) % 12;
        if(joueur == 1)
        {
            if(j > 5)
            {
                if(l[j] < 4 && l[j] > 1)
                {
                    *score1 += l[j];
                    l[j] = 0;
                }
                else break;
            }
            else break;
        }
        else // joueur 2
        {
            if(j < 6)
            {
                if(l[j] < 4 && l[j] > 1)
                {
                    *score2 += l[j];
                    l[j] = 0;
                }
                else break;
            }
            else break;
        }
    }
}


// Vérifie si le coup affame l’adversaire après simulation
int coup_valide(int l[12], int choix, int joueur, int score1, int score2)
{
    int copie[12];
    int s1 = score1, s2 = score2;
    copier_plateau(l, copie);

    jouer_coup(copie, choix, joueur, &s1, &s2);

    if (joueur == 1)
        return graines_restantes(copie, 6, 12) > 0; // l'adversaire (joueur2) doit avoir des graines
    else
        return graines_restantes(copie, 0, 6) > 0;  // l'adversaire (joueur1)
}




void find_game(Client* clients, int actual, int j)
{  
   Client *cli;
   int i = 0;
   char buffer[BUF_SIZE];
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(i != j)
      {
         if(clients[i].game.etat == 1)
         {
            cli = &clients[i];
            clients[i].game.etat = 2;
            clients[j].game.etat = 3;
            strcpy(clients[i].game.nameadv, clients[j].name);
            strcpy(clients[j].game.nameadv, clients[i].name);
            for(int z = 0; z<12; z++)
            {
               clients[i].game.l[z] = 4;
               clients[j].game.l[z] = 4;
            }
            clients[i].game.score = 0;
            clients[j].game.score = 0;
            
            strcpy(buffer, "\n\nPartie trouver tu commences!\n\nJoueur1: choisissez la case à repartir (1 à 6)\n\n");
            send_message_to_clients(clients, clients[i], actual, buffer);
            
            strcpy(buffer, "\n\nPartie trouver ton adversaire commences!\n\n");
            send_message_to_clients(clients, clients[j], actual, buffer);
            affichage(clients[i].game.l, buffer, BUF_SIZE);
            send_message_to_clients(clients, clients[i], actual, buffer);
            send_message_to_clients(clients, clients[j], actual, buffer);
            break;
         }
         
      }
   }
   return cli;
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

void send_message_to_clients(Client *clients, Client recever, int actual, const char *buffer)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      /* we send message to the sender */
      if(recever.sock == clients[i].sock)
      {
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
         break;
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
         
         strncpy(message, "serveur", BUF_SIZE - 1);
         strncat(message, " : ", sizeof message - strlen(message) - 1);
         
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
