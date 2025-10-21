
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


int affichage(int l[12])
{
    int i;
    for(i=0; i<6; i++)
    {
        printf("%d   ", l[i]);
    }
    printf("\n");
    for(i=6; i<12; i++)
    {
        printf("%d   ", l[i]);
    }
    printf("\n");
    return 0;
}

int main()
{
    int l[12];
    int joueur1=0, joueur2=0, i;
    for(i=0; i<12; i++)
    {
        l[i]=0;
    }
    affichage(l);
}