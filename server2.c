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
   char tmp[64];

   /* start with empty buffer */
   if (taille_buffer == 0) return -1;
   buffer[0] = '\0';

   /* Adversaire: */
   if (strlen(buffer) + strlen("Adversaire: ") < taille_buffer)
      strcpy(buffer, "Adversaire: ");
   for (i = 11; i >= 6; i--) {
      snprintf(tmp, sizeof tmp, "%3d ", l[i]);
      if (strlen(buffer) + strlen(tmp) < taille_buffer)
         strcat(buffer, tmp);
      else break;
   }
   if (strlen(buffer) + 1 < taille_buffer) strcat(buffer, "\n");

   /* ligne de séparation */
   if (strlen(buffer) + strlen("             ") < taille_buffer)
      strcat(buffer, "             ");
   for (i = 0; i < 6; i++) {
      if (strlen(buffer) + 4 < taille_buffer)
         strcat(buffer, "----");
      else break;
   }
   if (strlen(buffer) + 1 < taille_buffer) strcat(buffer, "\n");

   /* Vous: */
   if (strlen(buffer) + strlen("Vous:        ") < taille_buffer)
      strcat(buffer, "Vous:        ");
   for (i = 0; i < 6; i++) {
      snprintf(tmp, sizeof tmp, "%3d ", l[i]);
      if (strlen(buffer) + strlen(tmp) < taille_buffer)
         strcat(buffer, tmp);
      else break;
   }
   if (strlen(buffer) + 1 < taille_buffer) strcat(buffer, "\n");

   /* indices */
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


// int affichage(int l[12], char *buffer, size_t taille_buffer)
// {
//     int offset = 0;
//     int i;


    
//     offset += snprintf(buffer + offset, taille_buffer - offset, "Adversaire: ");
//     for (i = 11; i >= 6 && offset < (int)taille_buffer; i--) {
//         offset += snprintf(buffer + offset, taille_buffer - offset, "%3d ", l[i]);
//     }
//     offset += snprintf(buffer + offset, taille_buffer - offset, "\n");

    
//     offset += snprintf(buffer + offset, taille_buffer - offset, "             ");
//     for (i = 0; i < 6 && offset < (int)taille_buffer; i++) {
//         offset += snprintf(buffer + offset, taille_buffer - offset, "----");
//     }
//     offset += snprintf(buffer + offset, taille_buffer - offset, "\n");

    
//     offset += snprintf(buffer + offset, taille_buffer - offset, "Vous:        ");
//     for (i = 0; i < 6 && offset < (int)taille_buffer; i++) {
//         offset += snprintf(buffer + offset, taille_buffer - offset, "%3d ", l[i]);
//     }
//     offset += snprintf(buffer + offset, taille_buffer - offset, "\n");

    
//     offset += snprintf(buffer + offset, taille_buffer - offset, "             ");
//     for (i = 0; i < 6 && offset < (int)taille_buffer; i++) {
//         offset += snprintf(buffer + offset, taille_buffer - offset, "%3d ", i + 1);
//     }
//     offset += snprintf(buffer + offset, taille_buffer - offset, "\n");

//     return 0;
// }

// int affichage(int l[12], char *buffer, size_t taille_buffer)
// {
//     int offset = 0;
//     int i;


    
//     offset += snprintf(buffer + offset, taille_buffer - offset, "Adversaire: ");
//     for (i = 11; i >= 6 && offset < (int)taille_buffer; i--) {
//         offset += snprintf(buffer + offset, taille_buffer - offset, "%3d ", l[i]);
//     }
//     offset += snprintf(buffer + offset, taille_buffer - offset, "\n");

    
//     offset += snprintf(buffer + offset, taille_buffer - offset, "             ");
//     for (i = 0; i < 6 && offset < (int)taille_buffer; i++) {
//         offset += snprintf(buffer + offset, taille_buffer - offset, "----");
//     }
//     offset += snprintf(buffer + offset, taille_buffer - offset, "\n");

    
//     offset += snprintf(buffer + offset, taille_buffer - offset, "Vous:        ");
//     for (i = 0; i < 6 && offset < (int)taille_buffer; i++) {
//         offset += snprintf(buffer + offset, taille_buffer - offset, "%3d ", l[i]);
//     }
//     offset += snprintf(buffer + offset, taille_buffer - offset, "\n");

    
//     offset += snprintf(buffer + offset, taille_buffer - offset, "             ");
//     for (i = 0; i < 6 && offset < (int)taille_buffer; i++) {
//         offset += snprintf(buffer + offset, taille_buffer - offset, "%3d ", i + 1);
//     }
//     offset += snprintf(buffer + offset, taille_buffer - offset, "\n");

//     return 0;
// }


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

void copierinverse_plateau(int src[12], int dest[12])
{
    for (int i = 0; i < 6; i++) {
        dest[i] = src[6 + i];     
        dest[i + 6] = src[0 + i];  
    }
}



// Joue un coup (sans vérification), sur un plateau donné, pour un joueur donné
void jouer_coup_test(int l[12], int choix)
{
    int i, j, nb, pos;
    
    nb = l[choix-1];
    l[choix-1] = 0;

    for(i = 1; i < nb+1; i++)
    {
      pos = ((choix-1+i) % 12);
        
      if(((pos) == (choix-1)))
            continue;
            
      
      l[pos] += 1;
        
    }

    // Captures
    for(i = 0; i < nb; i++)
    {
        j = (choix -1 + nb - i) % 12;
            if(j > 5)
            {
               if(l[j] < 4 && l[j] > 1)
               {
                 l[j] = 0;
               }
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
      pos = ((choix-1+i) %  12);
        
      if(((pos) == (choix-1)))
            continue;
            
      
      printf("pos: %d\n", pos);
      clients[joueur1].game.l[pos] += 1;
        
    }

    // Captures
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


// Vérifie si le coup affame l’adversaire après simulation
int coup_valide(int l[12], int choix)
{
    int copie[12];

    copier_plateau(l, copie);

    jouer_coup_test(copie, choix);

    
   return (graines_restantes(copie, 6, 12) > 0); // l'adversaire (joueur2) doit avoir des graines
}


int find_player(Client* clients, int actual, int j)
{
   int i;
   char adv[BUF_SIZE];
   strcpy(adv, clients[j].game.nameadv);
   for(i = 0; i < actual; i++)
   {
      if(strcmp(clients[i].name, adv) == 0)
         {
            return i;
         } 
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
         {
            return i;
         } 
   }
   return -1;
}


void find_game(Client* clients, int actual, int j)
{  
   int i = 0;
   char buffer[BUF_SIZE];
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(i != j)
      {
         if(clients[i].etat == 1)
         {
            clients[i].etat = 2;
            clients[j].etat = 3;
            strcpy(clients[i].game.nameadv, clients[j].name);
            strcpy(clients[j].game.nameadv, clients[i].name);
            for(int z = 0; z<12; z++)
            {
               clients[i].game.l[z] = 4;
               clients[j].game.l[z] = 4;
            }
            clients[i].game.score = 0;
            clients[j].game.score = 0;
            
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
   return;
}


static void list_playing(char *buffer, size_t taille_buffer, int actual, Client* clients, int j)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
         if (clients[i].etat==2 || clients[i].etat==3 || clients[i].etat==6 || clients[i].etat==7 || clients[i].etat==8)
         {
            strncat(buffer, clients[i].name, BUF_SIZE - 1);
            strncat(buffer, " etat: ", BUF_SIZE - strlen(buffer) - 1);
            strncat(buffer, "en partie\n", BUF_SIZE - strlen(buffer) - 1);  
         }
   }

   return buffer;
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
         c.etat = 0;
         strncpy(c.name, buffer, BUF_SIZE - 1);
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
               /* a client is talking */
               if(FD_ISSET(clients[i].sock, &rdfs))
               {
                  Client client = clients[i];
                  int c = read_client(clients[i].sock, buffer);

                  etat = client.etat;
                  /* client disconnected */
                  if(c == 0)
                  {
                     closesocket(clients[i].sock);
                     if (clients[i].etat == 2 || clients[i].etat == 3)
                     {
                        int adv;
                        adv = find_player(clients, actual, i);
                        strcpy(buffer, "\n\nVotre adversaire s'est deconnecté! Tu as gagné!\n\n");
                        send_message_to_clients(clients, clients[adv], actual, buffer);  
                     }
                     remove_client(clients, i, &actual);
                     strncpy(buffer, client.name, BUF_SIZE - 1);
                     strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                     send_message_to_all_clients(clients, client, actual, buffer, 1);
                  }
                  else
                  {
                     //printf("%s\n", buffer);
                     nb = atoi(buffer);
                     //printf("%d\n", etat);
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
                           strcpy(buffer, "\n\nVoici la liste des joueurs en partie:?\n");
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
                           listerJoueurs(clients,actual,buffer,BUF_SIZE,i);
                           send_message_to_clients(clients, clients[i], actual, buffer);
                           strcpy(buffer, "\n\nIndique le nom de l'adversaire que tu souhaites défier: \n");
                           send_message_to_clients(clients, clients[i], actual, buffer);

                           int repRead = read_client(clients[i].sock, buffer);
                           defierJoueurSpe(buffer, clients,actual,i);
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
                           strcpy(buffer, "\n\nNous recherchons une partie\n\n");
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
                        strcpy(buffer, "\nEnvois c pour chatter\n");
                        send_message_to_clients(clients, clients[i], actual, buffer);
                     }
                     
                     else if (etat==5)
                     {
                        if (nb == 0)
                        {
                           strcpy(buffer, "\n\nTu es connecté au serveur, que veux tu faire, 1 pour joueur, 2 pour être spectateur d'une partie\n\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);  
                        }
                        int p = find_player_name(clients, actual, buffer);
                        if (p==-1 || (clients[p].etat!=2 && clients[p].etat!=3 && clients[p].etat!=6 && clients[p].etat!=7 && clients[p].etat!=8))
                        {
                           strcpy(buffer, "\nJoueur introuvable ou plus en partie\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                        }
                        clients[i].etat = 4;
                        strcpy(clients[i].namespec, buffer);
                        clients[p].game.spec = 1;

                        strcpy(buffer, "\nEnvois c pour chatter\n");
                        send_message_to_clients(clients, clients[i], actual, buffer);
                     }

                     else if (etat==6)
                     {
                        int choix = nb, adv;
                        //printf("%d\n", choix);
                        if(choix<1 || choix>6 || clients[i].game.l[choix-1]==0 || !coup_valide(clients[i].game.l, choix))//
                        {

                              if (choix<1 || choix>6 || clients[i].game.l[choix-1]==0)
                              {
                                 strcpy(buffer, "\nNombre invalide, veuillez reessayer\n");
                                 send_message_to_clients(clients, clients[i], actual, buffer);
                              }
                              
                                
                              else if (!coup_valide(clients[i].game.l, choix ))
                              {
                                 strcpy(buffer, "\nCe coup affame l’adversaire, choisissez une autre case\n");
                                 send_message_to_clients(clients, clients[i], actual, buffer);
                              }
                                
                        }
                        else
                        { 

                           adv = find_player(clients, actual, i);
                           jouer_coup(clients, choix, i, adv);
                           copierinverse_plateau(clients[i].game.l,clients[adv].game.l);
                           //send_message_to_clients(clients, clients[adv], actual, buffer);
                           affichage(clients[i].game.l, buffer, BUF_SIZE);
                           send_message_to_clients(clients, clients[i], actual, buffer);
                           strcpy(buffer, "\nCe coup a été joué, c'est à ton adversaire de jouer\n");
                           send_message_to_clients(clients, clients[i], actual, buffer);
                           //affichage(clients[adv].game.l, buffer, BUF_SIZE);
                           //send_message_to_clients(clients, clients[adv], actual, buffer);
                           if (clients[i].game.score > 25)
                           {
                              
                              snprintf(buffer, BUF_SIZE, "\nLa partie est gagnée ! Bravo, tu gagnes avec un score de %d et ton adversaire a %d\n", clients[i].game.score, clients[adv].game.score);
                              send_message_to_clients(clients, clients[i], actual, buffer);
                              strcpy(buffer, "\nCe coup a été joué\n");
                              send_message_to_clients(clients, clients[adv], actual, buffer);
                              affichage(clients[adv].game.l, buffer, BUF_SIZE);
                              send_message_to_clients(clients, clients[adv], actual, buffer);
                              snprintf(buffer, BUF_SIZE, "\nLa partie est perdu ! Ton adversaire a comme score: %d et toi: %d\n", clients[i].game.score, clients[adv].game.score);
                              send_message_to_clients(clients, clients[adv], actual, buffer);
                              clients[adv].etat = 0;
                              clients[i].etat = 0;
                              strcpy(buffer, "\n\nTu es connecté au serveur, que veux tu faire, 1 pour joeur, 2 pour être spectateur d'une partie\n\n");
                              send_message_to_clients(clients, clients[adv], actual, buffer);
                              send_message_to_clients(clients, clients[i], actual, buffer);  
                           }
                           else
                           {
                              snprintf(buffer, BUF_SIZE, "\nTon score: %d\n Score de ton adversaire: %d\n\nEnvois c pour chatter", clients[i].game.score, clients[adv].game.score);
                              send_message_to_clients(clients, clients[i], actual, buffer);
                              if (clients[adv].etat == 8)
                              {
                                 clients[adv].etat = 7;
                              }
                              else{
                                 clients[adv].etat = 2;
                              }
                              
                              clients[i].etat = 3;
                              strcpy(buffer, "\nC'est à toi de jouer revoici le plateau !\n");
                              send_message_to_clients(clients, clients[adv], actual, buffer);
                              affichage(clients[adv].game.l, buffer, BUF_SIZE);
                              send_message_to_clients(clients, clients[adv], actual, buffer);
                              snprintf(buffer, BUF_SIZE, "\nTon score: %d\n Score de ton adversaire: %d\n\nEnvois j pour jouer ou c pour chatter", clients[adv].game.score, clients[i].game.score);
                              send_message_to_clients(clients, clients[adv], actual, buffer);

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

void listerJoueurs(Client * client,int actual, char * buffer, size_t taille_buffer, int indiceCurrentJoueur)
{
   int offset = 0;
   int i;


   // Camp adverse
   for (i = 0; i < actual; i++)
   {
      if((client[i].etat == 0 || client[i].etat == 1) && indiceCurrentJoueur != i)
      {
         offset += snprintf(buffer + offset, taille_buffer - offset, client[i].name);
         offset += snprintf(buffer + offset, taille_buffer - offset, "\n");

      }
      
   }

   return;
}

void defierJoueurSpe(char * target, Client * client, int actual, int indiceCurrentJoueur)
{
   char buffer[BUF_SIZE];
   int iTargetPlayer = find_player_name(client, actual, target);

   if (iTargetPlayer == -1 || iTargetPlayer == indiceCurrentJoueur)
   {
      snprintf(buffer, BUF_SIZE, "\nJoueur introuvable ou pseudo invalide\n");
      send_message_to_clients(client, client[indiceCurrentJoueur], actual, buffer);
      return;
   }

   if (client[iTargetPlayer].etat != 1)
   {
      snprintf(buffer, BUF_SIZE, "\nLe joueur %s n'est pas disponible pour une partie\n", target);
      send_message_to_clients(client, client[indiceCurrentJoueur], actual, buffer);
      return;
   }


   client[iTargetPlayer].etat = 2;                        
   client[indiceCurrentJoueur].etat = 3;        

   strcpy(client[iTargetPlayer].game.nameadv, client[indiceCurrentJoueur].name);
   strcpy(client[indiceCurrentJoueur].game.nameadv, client[iTargetPlayer].name);

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
