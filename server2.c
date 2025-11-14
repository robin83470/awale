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
   char tmp[64];

   if (taille_buffer == 0) return -1;
   buffer[0] = '\0';

   if (strlen(buffer) + strlen("Adversaire: ") < taille_buffer)
      strcpy(buffer, "Adversaire: ");
   for (i = 11; i >= 6; i--) {
      snprintf(tmp, sizeof tmp, "%3d ", l[i]);
      if (strlen(buffer) + strlen(tmp) < taille_buffer)
         strcat(buffer, tmp);
      else break;
   }
   if (strlen(buffer) + 1 < taille_buffer) strcat(buffer, "\n");

   if (strlen(buffer) + strlen("             ") < taille_buffer)
      strcat(buffer, "             ");
   for (i = 0; i < 6; i++) {
      if (strlen(buffer) + 4 < taille_buffer)
         strcat(buffer, "----");
      else break;
   }
   if (strlen(buffer) + 1 < taille_buffer) strcat(buffer, "\n");

   if (strlen(buffer) + strlen("Vous:        ") < taille_buffer)
      strcat(buffer, "Vous:        ");
   for (i = 0; i < 6; i++) {
      snprintf(tmp, sizeof tmp, "%3d ", l[i]);
      if (strlen(buffer) + strlen(tmp) < taille_buffer)
         strcat(buffer, tmp);
      else break;
   }
   if (strlen(buffer) + 1 < taille_buffer) strcat(buffer, "\n");

   if (strlen(buffer) + strlen("             ") < taille_buffer)
      strcat(buffer, "             ");
   for (i = 0; i < 6; i++) {
      snprintf(tmp, sizeof tmp, "%3d ", i + 1);
      if (strlen(buffer) + strlen(tmp) < taille_buffer)
         strcat(buffer, tmp);
      else break;
   }
   if (strlen(buffer) + 1 < taille_buffer) strcat(buffer, "\n");

   return 0;
}

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

void copierinverse_plateau(int src[12], int dest[12])
{
    for (int i = 0; i < 6; i++) {
        dest[i] = src[6 + i];
        dest[i + 6] = src[i];
    }
}

void jouer_coup_test(int l[12], int choix)
{
    int i, j, nb, pos;

    nb = l[choix-1];
    l[choix-1] = 0;

    for(i = 1; i < nb+1; i++)
    {
      pos = ((choix-1+i) % 12);
      if(pos == choix-1) continue;
      l[pos] += 1;
    }

    for(i = 0; i < nb; i++)
    {
        j = (choix -1 + nb - i) % 12;
        if(j > 5)
        {
            if(l[j] < 4 && l[j] > 1)
                l[j] = 0;
            else break;
        }
        else break;
    }
}

void jouer_coup(Client * clients, int choix, int joueur1, int joueur2)
{
    int i, j, nb, pos;

    nb = clients[joueur1].game.l[choix-1];
    clients[joueur1].game.l[choix-1] = 0;

    for(i = 1; i < nb+1; i++)
    {
      pos = ((choix-1+i) % 12);
      if(pos == choix-1) continue;
      clients[joueur1].game.l[pos] += 1;
    }

    for(i = 0; i < nb; i++)
    {
        j = (choix -1 + nb - i) % 12;
        if(j > 5)
        {
            if(clients[joueur1].game.l[j] < 4 && clients[joueur1].game.l[j] > 1)
            {
                clients[joueur1].game.score += clients[joueur1].game.l[j];
                clients[joueur1].game.l[j] = 0;
            }
            else break;
        }
        else break;
    }
}

int coup_valide(int l[12], int choix)
{
    int copie[12];
    copier_plateau(l, copie);
    jouer_coup_test(copie, choix);
    return (graines_restantes(copie, 6, 12) > 0);
}

int find_player(Client* clients, int actual, int j)
{
   int i;
   char adv[BUF_SIZE];
   strcpy(adv, clients[j].game.nameadv);

   for(i = 0; i < actual; i++)
   {
      if(strcmp(clients[i].name, adv) == 0)
            return i;
   }
   return -1;
}

int find_player_name(Client* clients, int actual, char * name)
{
   int i;
   char p[BUF_SIZE];
   strcpy(p, name);

   for(i = 0; i < actual; i++)
   {
      if(strcmp(clients[i].name, p) == 0)
            return i;
   }
   return -1;
}

void find_game(Client* clients, int actual, int j)
{
   int i = 0;
   char buffer[BUF_SIZE];

   for(i = 0; i < actual; i++)
   {
      if(i != j && clients[i].etat == 1)
      {
         clients[i].etat = 2;
         clients[j].etat = 3;

         strcpy(clients[i].game.nameadv, clients[j].name);
         strcpy(clients[j].game.nameadv, clients[i].name);

         for(int z = 0; z < 12; z++)
         {
            clients[i].game.l[z] = 4;
            clients[j].game.l[z] = 4;
         }

         clients[i].game.score = 0;
         clients[j].game.score = 0;
         clients[i].game.spec = 0;
         clients[j].game.spec = 0;

         strcpy(buffer, "\n\nPartie trouver tu commences!\n\nEnvois j pour jouer ou c pour chatter\n\n");
         send_message_to_clients(clients, clients[i], actual, buffer);

         strcpy(buffer, "\n\nPartie trouver ton adversaire commences!\n\nEnvois c pour chatter");
         send_message_to_clients(clients, clients[j], actual, buffer);

         affichage(clients[i].game.l, buffer, BUF_SIZE);
         send_message_to_clients(clients, clients[i], actual, buffer);
         send_message_to_clients(clients, clients[j], actual, buffer);

         break;
      }
   }
}

static void list_playing(char *buffer, size_t taille_buffer, int actual, Client* clients, int j)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
         if (clients[i].etat==2 || clients[i].etat==3 || clients[i].etat==6 ||
             clients[i].etat==7 || clients[i].etat==8)
         {
            strncat(buffer, clients[i].name, BUF_SIZE - 1);
            strncat(buffer, " etat: ", BUF_SIZE - strlen(buffer) - 1);
            strncat(buffer, "en partie\n", BUF_SIZE - strlen(buffer) - 1);
         }
   }
}

static void app(void)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   int actual = 0;
   int max = sock;
   Client clients[MAX_CLIENTS];
   fd_set rdfs;

   while(1)
   {
      int i = 0;
      FD_ZERO(&rdfs);
      FD_SET(STDIN_FILENO, &rdfs);
      FD_SET(sock, &rdfs);

      for(i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         SOCKADDR_IN csin = { 0 };
         size_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);

         if(csock == SOCKET_ERROR)
            continue;

         if(read_client(csock, buffer) == -1)
            continue;

         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client c = { csock };
         c.etat = 0;
         strcpy(c.namespec, "0");
         strncpy(c.name, buffer, 50);
         clients[actual] = c;
         actual++;

         strcpy(buffer, "\n\nTu es connecté au serveur, que veux tu faire, 1 pour jouer, 2 pour être spectateur d'une partie, 3 defier un joueur en particulier\n\n");
         send_message_to_clients(clients, c, actual, buffer);
      }

      else
      {
         int i = 0;
         int etat, nb;

         for(i = 0; i < actual; i++)
         {
            if(FD_ISSET(clients[i].sock, &rdfs))
            {
               Client client = clients[i];
               int c = read_client(clients[i].sock, buffer);

               etat = client.etat;

               if(c == 0)
               {
                  char leaver[BUF_SIZE];
                  char adv_name[BUF_SIZE];
                  char spec_name[BUF_SIZE];

                  strncpy(leaver, client.name, BUF_SIZE - 1);
                  leaver[BUF_SIZE - 1] = '\0';

                  strncpy(adv_name, client.game.nameadv, BUF_SIZE - 1);
                  adv_name[BUF_SIZE - 1] = '\0';

                  strncpy(spec_name, client.namespec, BUF_SIZE - 1);
                  spec_name[BUF_SIZE - 1] = '\0';

                  closesocket(clients[i].sock);

                  if (etat == 2 || etat == 3 || etat == 6 || etat == 7 || etat == 8)
                  {
                        int adv = find_player(clients, actual, i);

                        if (adv != -1)
                        {
                           snprintf(buffer, BUF_SIZE, "\n\nVotre adversaire %s s'est deconnecté! La partie est terminée. Tu reviens au menu principal.\n\n", leaver);
                           send_message_to_clients(clients, clients[adv], actual, buffer);

                           clients[adv].etat = 0;
                           for (int z = 0; z < 12; z++) clients[adv].game.l[z] = 0;
                           clients[adv].game.score = 0;
                           clients[adv].game.spec = 0;
                        }

                        for (int k = 0; k < actual; k++)
                        {
                           if (clients[k].etat == 4 && strcmp(clients[k].namespec, leaver) == 0)
                           {
                              snprintf(buffer, BUF_SIZE, "\nLe joueur %s que tu observais s'est deconnecté. Observation terminée. Tu reviens au menu principal.\n", leaver);
                              send_message_to_clients(clients, clients[k], actual, buffer);
                              clients[k].etat = 0;
                              clients[k].namespec[0] = '\0';
                           }
                        }
                  }

                  if (etat == 10)
                  {
                        if (adv_name[0] != '\0')
                        {
                           int target = find_player_name(clients, actual, adv_name);
                           if (target != -1)
                           {
                              snprintf(buffer, BUF_SIZE, "\nLe joueur %s qui t'a défié s'est déconnecté. La demande est annulée.\n", leaver);
                              send_message_to_clients(clients, clients[target], actual, buffer);
                              clients[target].etat = 0;
                           }
                        }
                  }

                  if (etat == 11)
                  {
                        if (adv_name[0] != '\0')
                        {
                           int challenger = find_player_name(clients, actual, adv_name);
                           if (challenger != -1)
                           {
                              snprintf(buffer, BUF_SIZE, "\nLe joueur %s à qui tu as envoyé un défi s'est déconnecté. La demande est annulée.\n", leaver);
                              send_message_to_clients(clients, clients[challenger], actual, buffer);
                              clients[challenger].etat = 0;
                           }
                        }
                  }

                  if (etat == 4 || etat == 12)
                  {
                        if (spec_name[0] != '\0')
                        {
                           int p = find_player_name(clients, actual, spec_name);
                           if (p != -1)
                              clients[p].game.spec = 0;
                        }
                  }

                  remove_client(clients, i, &actual);
                  break;
               }
               else
               {
                  nb = atoi(buffer);
                                    if (etat==0)
                  {
                        if(nb == 1)
                        {
                           clients[i].etat = 1;
                           strcpy(buffer, "\n\nNous cherchons une partie, 0 pour annuler\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                           find_game(clients, actual, i);
                        }
                        else if(nb == 2)
                        {
                           clients[i].etat = 5;
                           strcpy(buffer, "\n\nVoici la liste des joueurs en partie:\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                           list_playing(buffer, BUF_SIZE, actual, clients, i);
                           send_message_to_clients(clients, clients[i], actual, buffer);
                           strcpy(buffer, "\nEnvois le nom du joueur que tu veux observer ou 0 pour annuler\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                        }
                        else if (nb == 3)
                        {
                           strcpy(buffer, "\n\nVoici la liste des joueurs disponibles:\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                           listerJoueurs(clients, actual, buffer, BUF_SIZE, i);
                           send_message_to_clients(clients, clients[i], actual, buffer);
                           strcpy(buffer, "\n\nIndique le nom de l'adversaire que tu souhaites défier (ou 0 pour annuler): \n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                           clients[i].etat = 9;
                        }
                        else
                        {
                           strcpy(buffer, "\nMessage non compris\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                        }
                  }

                  else if (etat==1)
                  {
                        if(nb == 0)
                        {
                           clients[i].etat = 0;
                           strcpy(buffer, "\n\nRecherche annuler\nTu es connecté au serveur, que veux tu faire, 1 pour jouer, 2 pour être spectateur d'une partie, 3 pour défier\n\n");
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
                        if(strcmp("c", buffer)==0)
                        {
                           clients[i].etat = 7;
                           strcpy(buffer, "\n\nMessage à envoyer?\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                        }
                        else if (strcmp("j", buffer)==0)
                        {
                           clients[i].etat = 6;
                           strcpy(buffer, "\n\nCoup à jouer(1 à 6)?\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                        }
                        else
                        {
                           strcpy(buffer, "\n\nMessage non compris, envois j pour jouer et c pour chatter\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                        }
                  }

                  else if (etat==3)
                  {
                        if(strcmp("c", buffer)==0)
                        {
                           clients[i].etat = 8;
                           strcpy(buffer, "\n\nMessage à envoyer?\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                        }
                        else
                        {
                           strcpy(buffer, "\n\nMessage non compris, envois c pour chatter\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                        }
                  }

                  else if (etat==4)
                  {
                        if (strcmp("0", buffer)==0)
                        {
                           int p = find_player_name(clients, actual, clients[i].namespec);
                           if (p != -1) clients[p].game.spec = 0;
                           clients[i].etat = 0;
                           strcpy(buffer, "\n\nTu es connecté au serveur, que veux tu faire, 1 pour joueur, 2 pour être spectateur d'une partie, 3 pour défier\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                        }
                        else if(strcmp("c", buffer)==0)
                        {
                           clients[i].etat = 12;
                           strcpy(buffer, "\n\nMessage à envoyer?\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                        }
                        else
                        {
                           strcpy(buffer, "\n\nMessage non compris, envois 0 pour annuler et c pour chatter\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                        }
                  }

                  else if (etat==12)
                  {
                        if (strcmp("0", buffer) == 0)
                        {
                           clients[i].etat = 0;
                           strcpy(clients[i].namespec, "0");
                           strcpy(buffer, "\n\nTu es connecté au serveur, que veux tu faire, 1 pour joueur, 2 pour être spectateur d'une partie, 3 pour défier\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                           break;
                        }

                        int spec = find_player_name(clients, actual, clients[i].namespec);
                        int adv = find_player(clients, actual, spec);

                        char message[BUF_SIZE];
                        message[0] = 0;

                        clients[i].etat = 4;

                        strncpy(message, clients[i].name, BUF_SIZE - 1);
                        strncat(message, "(spec) : ", sizeof message - strlen(message) - 1);
                        strncat(message, buffer, sizeof message - strlen(message) - 1);

                        send_message_to_clients(clients, clients[spec], actual, message);
                        send_message_to_clients(clients, clients[adv], actual, message);

                        strcpy(buffer, "\nEnvois 0 pour arreter d'observer et c pour chatter\n");
                        send_message_to_clients(clients, clients[i], actual, buffer);
                  }

                  else if (etat==7)
                  {
                        int adv = find_player(clients, actual, i);
                        char message[BUF_SIZE];
                        message[0] = 0;

                        clients[i].etat = 2;

                        strncpy(message, clients[i].name, BUF_SIZE - 1);
                        strncat(message, " : ", sizeof message - strlen(message) - 1);
                        strncat(message, buffer, sizeof message - strlen(message) - 1);

                        send_message_to_clients(clients, clients[adv], actual, message);

                        if(clients[i].game.spec == 1)
                           send_message_spec(clients, clients[i], actual, message, i);

                        if(clients[adv].game.spec == 1)
                           send_message_spec(clients, clients[adv], actual, message, adv);

                        strcpy(buffer, "\nEnvois j pour jouer ou c pour chatter\n");
                        send_message_to_clients(clients, clients[i], actual, buffer);
                  }

                  else if (etat==8)
                  {
                        int adv = find_player(clients, actual, i);
                        char message[BUF_SIZE];
                        message[0] = 0;

                        clients[i].etat = 3;

                        strncpy(message, clients[i].name, BUF_SIZE - 1);
                        strncat(message, " : ", sizeof message - strlen(message) - 1);
                        strncat(message, buffer, sizeof message - strlen(message) - 1);

                        send_message_to_clients(clients, clients[adv], actual, message);

                        if(clients[i].game.spec == 1)
                           send_message_spec(clients, clients[i], actual, message, i);

                        if(clients[adv].game.spec == 1)
                           send_message_spec(clients, clients[adv], actual, message, adv);

                        strcpy(buffer, "\nEnvois c pour chatter\n");
                        send_message_to_clients(clients, clients[i], actual, buffer);
                  }

                  else if (etat==5)
                  {
                        char target[BUF_SIZE];
                        strncpy(target, buffer, BUF_SIZE - 1);
                        target[BUF_SIZE - 1] = '\0';

                        if (strcmp(target, "0") == 0)
                        {
                           clients[i].etat = 0;
                           strcpy(buffer, "\n\nTu es connecté au serveur, que veux tu faire, 1 pour jouer, 2 pour être spectateur d'une partie, 3 defier un joueur en particulier\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                           clients[i].etat = 0;
                        }
                        else
                        {
                           int p = find_player_name(clients, actual, target);

                           if (p == -1 || (clients[p].etat != 2 && clients[p].etat != 3 && clients[p].etat != 6 && clients[p].etat != 7 && clients[p].etat != 8))
                           {
                              strcpy(buffer, "\nJoueur introuvable ou plus en partie\n");
                              send_message_to_clients(clients, clients[i], actual, buffer);
                              strcpy(buffer, "\nEnvois le nom du joueur que tu veux observer ou 0 pour annuler\n");
                              send_message_to_clients(clients, clients[i], actual, buffer);
                           }
                           else
                           {
                              clients[i].etat = 4;
                              strncpy(clients[i].namespec, target, BUF_SIZE - 1);
                              clients[p].game.spec = 1;

                              int adv = find_player(clients, actual, p);

                              snprintf(buffer, BUF_SIZE, "\nTu observes maintenant %s. Envois c pour chatter avec les joueurs observés ou 0 pour quitter l'observation\n", clients[p].name);
                              send_message_to_clients(clients, clients[i], actual, buffer);

                              snprintf(buffer, BUF_SIZE, "\nVoici l'état de la partie, score de %s: %d, score de %s: %d\n", clients[p].name, clients[p].game.score, clients[adv].name, clients[p].game.score);
                              send_message_to_clients(clients, clients[i], actual, buffer);

                              affichage(clients[p].game.l, buffer, BUF_SIZE);
                              send_message_to_clients(clients, clients[i], actual, buffer);
                           }
                        }
                  }

                  else if (etat==6)
                  {
                        int choix = nb;
                        int adv;

                        if(choix<1 || choix>6 || clients[i].game.l[choix-1]==0 || !coup_valide(clients[i].game.l, choix))
                        {
                              if (choix<1 || choix>6 || clients[i].game.l[choix-1]==0)
                              {
                                 strcpy(buffer, "\nNombre invalide, veuillez reessayer\n");
                                 send_message_to_clients(clients, clients[i], actual, buffer);
                              }
                              else
                              {
                                 strcpy(buffer, "\nCe coup affame l'adversaire, choisissez une autre case\n");
                                 send_message_to_clients(clients, clients[i], actual, buffer);
                              }
                        }
                        else
                        {
                           adv = find_player(clients, actual, i);
                           jouer_coup(clients, choix, i, adv);
                           copierinverse_plateau(clients[i].game.l,clients[adv].game.l);

                           affichage(clients[i].game.l, buffer, BUF_SIZE);
                           send_message_to_clients(clients, clients[i], actual, buffer);

                           if( clients[i].game.spec == 1)
                           {
                              send_message_spec(clients, clients[i], actual, buffer, i);
                              snprintf(buffer, BUF_SIZE, "\nCe coup a été joué par %s(0 pour arreter d'observer et c pour chatter)\n", clients[i].name);
                              send_message_spec(clients, clients[i], actual, buffer, i);
                           }

                           strcpy(buffer, "\nCe coup a été joué, c'est à ton adversaire de jouer\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);

                           if (clients[i].game.score > 25)
                           {
                              if( clients[i].game.spec == 1)
                              {
                                 snprintf(buffer, BUF_SIZE, "\nLa partie est finie ! Bravo, %s gagnes avec un score de %d et %s a %d(appuie sur 0 pour te deconnecter)\n",  clients[i].name, clients[i].game.score, clients[adv].name, clients[adv].game.score);
                                 send_message_spec(clients, clients[i], actual, buffer, i);
                              }

                              snprintf(buffer, BUF_SIZE, "\nLa partie est gagnée ! Bravo, tu gagnes avec un score de %d et ton adversaire a %d\n", clients[i].game.score, clients[adv].game.score);
                              send_message_to_clients(clients, clients[i], actual, buffer);

                              strcpy(buffer, "\nCe coup a été joué\n");
                              send_message_to_clients(clients, clients[adv], actual, buffer);

                              affichage(clients[adv].game.l, buffer, BUF_SIZE);
                              send_message_to_clients(clients, clients[adv], actual, buffer);

                              snprintf(buffer, BUF_SIZE, "\nLa partie est perdu ! Ton adversaire a comme score: %d et toi: %d\n", clients[i].game.score, clients[adv].game.score);
                              send_message_to_clients(clients, clients[adv], actual, buffer);

                              if( clients[adv].game.spec == 1)
                              {
                                 strcpy(buffer, "\nCe coup a été joué\n");
                                 send_message_spec(clients, clients[adv], actual, buffer, i);

                                 affichage(clients[adv].game.l, buffer, BUF_SIZE);
                                 send_message_spec(clients, clients[adv], actual, buffer, i);

                                 snprintf(buffer, BUF_SIZE, "\nLa partie est finie ! Bravo, %s gagnes avec un score de %d et %s a %d(appuie sur 0 pour te deconnecter)\n",  clients[i].name, clients[i].game.score, clients[adv].name, clients[adv].game.score);
                                 send_message_spec(clients, clients[adv], actual, buffer, i);
                              }

                              clients[adv].etat = 0;
                              clients[i].etat = 0;

                              strcpy(buffer, "\n\nTu es connecté au serveur, que veux tu faire, 1 pour joeur, 2 pour être spectateur d'une partie\n\n");
                              send_message_to_clients(clients, clients[adv], actual, buffer);
                              send_message_to_clients(clients, clients[i], actual, buffer);
                           }
                           else
                           {
                              snprintf(buffer, BUF_SIZE, "\nTon score: %d\nScore de ton adversaire: %d\n\nEnvois c pour chatter", clients[i].game.score, clients[adv].game.score);
                              send_message_to_clients(clients, clients[i], actual, buffer);

                              if( clients[i].game.spec == 1)
                              {
                                 snprintf(buffer, BUF_SIZE, "\nScore de %s: %d\nScore de %s: %d\n(0 pour annuler l'observation et c pour chatter)\n", clients[i].name, clients[i].game.score, clients[adv].name, clients[adv].game.score);
                                 send_message_spec(clients, clients[i], actual, buffer, i);
                              }

                              if (clients[adv].etat == 8)
                                 clients[adv].etat = 7;
                              else
                                 clients[adv].etat = 2;

                              clients[i].etat = 3;

                              strcpy(buffer, "\nC'est à toi de jouer revoici le plateau !\n");
                              send_message_to_clients(clients, clients[adv], actual, buffer);

                              affichage(clients[adv].game.l, buffer, BUF_SIZE);
                              send_message_to_clients(clients, clients[adv], actual, buffer);

                              snprintf(buffer, BUF_SIZE, "\nTon score: %d\nScore de ton adversaire: %d\n\nEnvois j pour jouer ou c pour chatter", clients[adv].game.score, clients[i].game.score);
                              send_message_to_clients(clients, clients[adv], actual, buffer);

                              if( clients[adv].game.spec == 1)
                              {
                                 strcpy(buffer, "\nC'est à toi de jouer revoici le plateau !\n");
                                 send_message_spec(clients, clients[adv], actual, buffer, i);

                                 affichage(clients[adv].game.l, buffer, BUF_SIZE);
                                 send_message_spec(clients, clients[adv], actual, buffer, i);

                                 snprintf(buffer, BUF_SIZE, "\nScore de %s: %d\nScore de %s: %d\n(appuie sur 0 pour te deconnecter et c pour chatter)\n",  clients[i].name, clients[i].game.score, clients[adv].name, clients[adv].game.score);
                                 send_message_spec(clients, clients[adv], actual, buffer, i);
                              }
                           }
                        }
                  }
                                  else if (etat == 9)
                  {
                        char target[BUF_SIZE];
                        int p;

                        strncpy(target, buffer, BUF_SIZE - 1);
                        target[BUF_SIZE - 1] = '\0';

                        if (strcmp(target, "0") == 0)
                        {
                           strcpy(buffer, "\nDéfi annulé.\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);

                           clients[i].etat = 0;

                           strcpy(buffer, "\n\nTu es connecté au serveur, que veux tu faire, 1 pour jouer, 2 pour être spectateur d'une partie, 3 defier un joueur en particulier\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                           break;
                        }

                        p = find_player_name(clients, actual, target);

                        if (p == -1 || (clients[p].etat != 0 && clients[p].etat != 1 && clients[p].etat != 5))
                        {
                           strcpy(buffer, "\nJoueur introuvable ou non disponible pour une partie\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);

                           clients[i].etat = 0;

                           strcpy(buffer, "\n\nTu es connecté au serveur, que veux tu faire, 1 pour jouer, 2 pour être spectateur d'une partie, 3 defier un joueur en particulier\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                           break;
                        }

                        snprintf(buffer, BUF_SIZE, "\nVous avez reçu une demande de défi de %s.\nTapez 1 pour accepter, 0 pour refuser :\n", clients[i].name);
                        send_message_to_clients(clients, clients[p], actual, buffer);

                        snprintf(buffer, BUF_SIZE, "\nDemande de défi envoyée à %s. En attente de sa réponse...\n", clients[p].name);
                        send_message_to_clients(clients, clients[i], actual, buffer);

                        clients[i].etat = 10;
                        strncpy(clients[i].game.nameadv, target, BUF_SIZE - 1);

                        clients[p].etat = 11;
                        strncpy(clients[p].game.nameadv, clients[i].name, BUF_SIZE - 1);
                  }

                  else if (etat == 10)
                  {
                        strcpy(buffer, "\nEn attente de réponse...\n");
                        send_message_to_clients(clients, clients[i], actual, buffer);
                  }

                  else if (etat == 11)
                  {
                        int challenger_idx = find_player_name(clients, actual, clients[i].game.nameadv);
                        int resp = nb;

                        if (resp == 1)
                        {
                           if (challenger_idx != -1)
                           {
                              clients[challenger_idx].etat = 2;
                              clients[i].etat = 3;

                              for (int z = 0; z < 12; z++)
                              {
                                 clients[challenger_idx].game.l[z] = 4;
                                 clients[i].game.l[z] = 4;
                              }

                              clients[challenger_idx].game.score = 0;
                              clients[i].game.score = 0;

                              snprintf(buffer, BUF_SIZE, "\n\nPartie trouvée, tu commences!\n\nEnvois j pour jouer ou c pour chatter\n\n");
                              send_message_to_clients(clients, clients[challenger_idx], actual, buffer);

                              snprintf(buffer, BUF_SIZE, "\n\nPartie trouvée, ton adversaire commence!\n\nEnvois c pour chatter\n");
                              send_message_to_clients(clients, clients[i], actual, buffer);

                              affichage(clients[challenger_idx].game.l, buffer, BUF_SIZE);
                              send_message_to_clients(clients, clients[challenger_idx], actual, buffer);
                              send_message_to_clients(clients, clients[i], actual, buffer);
                           }
                        }
                        else
                        {
                           if (challenger_idx != -1)
                           {
                              snprintf(buffer, BUF_SIZE, "\n%s a refusé votre défi.\n", clients[i].name);
                              send_message_to_clients(clients, clients[challenger_idx], actual, buffer);
                              clients[challenger_idx].etat = 0;
                           }

                           strcpy(buffer, "\nVous avez refusé le défi.\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                           clients[i].etat = 0;

                           strcpy(buffer, "\n\nTu es connecté au serveur, que veux tu faire, 1 pour jouer, 2 pour être spectateur d'une partie, 3 defier un joueur en particulier\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);

                           if (challenger_idx != -1)
                           {
                              strcpy(buffer, "\n\nTu es connecté au serveur, que veux tu faire, 1 pour jouer, 2 pour être spectateur d'une partie, 3 defier un joueur en particulier\n\n");
                              send_message_to_clients(clients, clients[challenger_idx], actual, buffer);
                           }
                        }
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

static void clear_clients(Client *clients, int actual)
{
   for(int i = 0; i < actual; i++)
      closesocket(clients[i].sock);
}

static void remove_client(Client *clients, int to_remove, int *actual)
{
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   (*actual)--;
}

void send_message_to_clients(Client *clients, Client recever, int actual, const char *buffer)
{
   char message[BUF_SIZE];
   message[0] = 0;

   for(int i = 0; i < actual; i++)
   {
      if(recever.sock == clients[i].sock)
      {
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
         break;
      }
   }
}

void send_message_spec(Client *clients, Client player, int actual, const char *message, int j)
{
   for(int i = 0; i < actual; i++)
   {
      if (i != j )
      {
         if(strcmp(player.name, clients[i].namespec) == 0)
            write_client(clients[i].sock, message);
      }
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
   int n = recv(sock, buffer, BUF_SIZE - 1, 0);

   if(n < 0)
   {
      perror("recv()");
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

void listerJoueurs(Client * client, int actual, char * buffer, size_t taille_buffer, int indiceCurrentJoueur)
{
   int offset = 0;

   for (int i = 0; i < actual; i++)
   {
      if ((client[i].etat == 1 || client[i].etat == 0) && indiceCurrentJoueur != i)
         offset += snprintf(buffer + offset, taille_buffer - offset, "  - %s\n", client[i].name);
   }

   if (offset == 0)
      snprintf(buffer, taille_buffer, "Aucun joueur disponible pour s'affronter.\n");
}

void defierJoueurSpe(char * target, Client * client, int actual, int indiceCurrentJoueur)
{
   char buffer[BUF_SIZE];
   char tgt[BUF_SIZE];

   strncpy(tgt, target, BUF_SIZE - 1);
   tgt[BUF_SIZE - 1] = '\0';

   int iTargetPlayer = find_player_name(client, actual, tgt);

   if (iTargetPlayer == -1 || iTargetPlayer == indiceCurrentJoueur)
   {
      snprintf(buffer, BUF_SIZE, "\nJoueur introuvable ou pseudo invalide\n");
      send_message_to_clients(client, client[indiceCurrentJoueur], actual, buffer);
      return;
   }

   if (client[iTargetPlayer].etat != 0 && client[iTargetPlayer].etat != 1)
   {
      snprintf(buffer, BUF_SIZE, "\nLe joueur %s n'est pas disponible pour une partie\n", tgt);
      send_message_to_clients(client, client[indiceCurrentJoueur], actual, buffer);
      return;
   }

   client[iTargetPlayer].etat = 2;
   client[indiceCurrentJoueur].etat = 3;

   strncpy(client[iTargetPlayer].game.nameadv, client[indiceCurrentJoueur].name, BUF_SIZE - 1);
   strncpy(client[indiceCurrentJoueur].game.nameadv, client[iTargetPlayer].name, BUF_SIZE - 1);

   for (int z = 0; z < 12; z++)
   {
      client[iTargetPlayer].game.l[z] = 4;
      client[indiceCurrentJoueur].game.l[z] = 4;
   }

   client[iTargetPlayer].game.score = 0;
   client[indiceCurrentJoueur].game.score = 0;

   snprintf(buffer, BUF_SIZE, "\n\nPartie trouvée, tu commences!\n\nEnvois j pour jouer ou c pour chatter\n\n");
   send_message_to_clients(client, client[iTargetPlayer], actual, buffer);

   snprintf(buffer, BUF_SIZE, "\n\nPartie trouvée, ton adversaire commence!\n\nEnvois c pour chatter\n");
   send_message_to_clients(client, client[indiceCurrentJoueur], actual, buffer);

   affichage(client[iTargetPlayer].game.l, buffer, BUF_SIZE);
   send_message_to_clients(client, client[iTargetPlayer], actual, buffer);
   send_message_to_clients(client, client[indiceCurrentJoueur], actual, buffer);
}


int main(int argc, char **argv)
{
   init();
   app();
   end();
   return EXIT_SUCCESS;
}
