
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
    for(i=11; i>5; i--)
    {
        printf("%d   ", l[i]);
    }
    printf("\n");
    return 0;
}

int main()
{
    int l[12];
    int joueur1=0, joueur2=0, i, nb;
    for(i=0; i<12; i++)
    {
        l[i]=4;
    }

    while (joueur1 < 25 && joueur2<25)
    {
        printf("\n\n NOUVEAU TOUR \n\n");

        printf("Point du joueur1: %d\nPoint du joueur2: %d\n", joueur1, joueur2);
        affichage(l);
        int choix=0;
        
        printf("Joueur1: choisissez la case à repartir (1 à 6)\n");
        while(choix<1 || choix>6)
        {
            scanf("%d", &choix);
            if (choix<1 || choix>6)
            {
                printf("Nombre invalide veuillez reessayer\n");
            }
        }
        nb = l[choix-1];
        l[choix-1] = 0;
        for(i = 1; i<nb+1; i++)
        {

            
            if(choix+i-1 > 11)
            {
                if((choix-1+i -11) == (choix-1) )
                {
                    continue;
                }
                l[choix-1+i -11 ] += 1;
            }
            
            else
            {
                l[choix-1+i] += 1;
            }
            
        }
        int j=0;
        for(i = 0; i<nb; i++)
        {
            j = (choix + nb - i)%12;
            if( j > 5)
            {
                if(l[j]<4 && l[j]>1)
                {
                    joueur1 += l[j];
                    l[j] = 0;
                }
                else
                {
                    break;
                }
            }
            else{
                break;
            }
        
        }


        printf("\n\n");
        affichage(l);
        choix=0;
        
        printf("Joueur2: choisissez la case à repartir (7 à 12)\n");
        while(choix<7 || choix>12 || l[choix]==0)
        {
            scanf("%d", &choix);
            if (choix<7 || choix>12 || l[choix]==0)
            {
                printf("Nombre invalide veuillez reessayer\n");
            }
        }
        nb = l[choix-1];
        l[choix-1] = 0;
        for(i = 1; i<nb+1; i++)
        {
             j = ((choix-1 + i)%12);
            
                
                if(j == (choix-1) )
                {
                    continue;
                }
                l[j] += 1;
            
        }
        
        for(i = 0; i<nb; i++)
        {
            j = ((choix-1 + nb - i)%12);
            if(  j<6 )
            {
                if(l[j]<4 && l[j]>1)
                {
                    joueur1 += l[j];
                    l[j] = 0;
                }
                else
                {
                    break;
                }
            }
            else{
                break;
            }
            
        }
    }
}