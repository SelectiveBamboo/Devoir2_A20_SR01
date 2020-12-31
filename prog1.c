#include <stdio.h>
#include <error.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

#define FILENAME "orderedProc.temp"

/*
    Programme créant un arbre de processus correspondant à l'exercice 1 du TP2 de SR01.
    La création des processus se fait dans l'ordre indiqué sur le schéma du TP.
    On passe par un fichier tampon {FILENAME} pour coordonner les processus, chacun attendant
    un certain nombre de lignes dans ce fichier (ex: 4 lignes = les processus 1, 2, 3 et 4 se sont lancés)
    avant de créer ses fils.
*/

int writeNumInFile(int n) 
//Ecris le numéro du processus (n) dans le fichier et créé une nouvelle ligne
{
    FILE * fp;

    fp = fopen (FILENAME, "a");

    if(fp==NULL) 
   {
      fprintf(stderr, "Problème à l'écriture du fichier \"%s\" ! Blocage du programme\n",FILENAME);
      exit(EXIT_FAILURE);
   }

    fprintf(fp, "%d\n", n);

    fclose(fp);

    return 0;
}

int countLinesInFile() 
//Compte le nombre de lignes dans le fichier
{
   FILE *fp;
   char ch;
   int linesCount=0;

   fp=fopen(FILENAME,"r");
   if(fp==NULL) 
   {
      fprintf(stderr, "Le fichier \"%s\" n'existe pas ! Blocage du programme\n",FILENAME);
      exit(EXIT_FAILURE);
   }

   while((ch=fgetc(fp))!=EOF) 
   {
      if(ch=='\n')
         linesCount++;
   }
   fclose(fp);

   return linesCount;
}


void forking(int n)
//méthode forkant chaque processus e nombre de fois voulu, au moment nécessaire
{
    int pid;
    int start;
    int end;
    int i;

    switch (n)  // Selon le processus actuel (n), on créera un certain nombre de fils
    {
        case 1:
            start = 2;
            end = 5;
            break;

        case 2:
            start = 6;
            end = 8;
            break;
        
        case 3:
            start = 9;
            end = 10;
            break;
        
        case 4:
            start = 11;
            end = 11;
            break;

        case 6:
            start = 12;
            end = 13;
            break;

        case 7:
            start = 14;
            end = 14;
            break;

        case 9:
            start = 15;
            end = 15;
            break;

        case 12:
            start = 16;
            end = 16;
            break;    
        
        default:    // Si default on est dans un processus qui ne crée pas de fils
            start = 1;
            end = 0;
            break;
    }

    int numOfLines = countLinesInFile();
    
    i = 0;
    while (numOfLines != (n-1)) //Permet l'ordonnancement des processus (Voir commentaire L.10)
    {
        if (!(numOfLines >= 0))
        {
            perror("Problème avec le fichier de suivi des processus\n");
            exit(EXIT_FAILURE);
        }
        if (i > 1000)
        {
            perror("Limite de temps atteinte, arrêt pour ne pas se bloquer\n");
            exit(EXIT_FAILURE);
        }
        

        numOfLines = countLinesInFile();
        i++;
        usleep(5000);
    }
    writeNumInFile(n);  //Déclare dans le fichier de suivi des processus qu'il est désormais actif
                
    //Création de {start - end} fils, chacun ayant leur numéro d'ordre (i) correspondant au schéma du TP
    for (i = start; i <= end; i++)  
    {
        pid = fork();
        if ( pid == -1) //Si problème pour créer le fils
        {
            perror("Impossible de créer le fils\n");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) //Si on est dans le fils
        {
            printf("Ici %d, mon pid est %d, le pid de mon père %d est %d \n", i, getpid(), n, getppid());
            forking(i); //Le fils va potentiellement créer ses propres fils
            break;
        }
    }

    if(pid != 0)
    {
        while( i > start)   //Attend la mort de ses fils
        {
            if (waitpid(-1, NULL, WNOHANG) > 0)
                i--;

            usleep(10000);
        }
    }

    //sleep(5);  //Pour une observation plus facile avec par exemple:  ps -l --forest -C "prog1,bash,upstart" 
    exit(0);
}

int main(int argc, char const *argv[])
{
    FILE * f = fopen (FILENAME, "w");     //Ecrase le fichier si existant
    if(f==NULL) 
    {
        fprintf(stderr, "Problème avec le fichier \"%s\" ! Fin.\n",FILENAME);
        exit(EXIT_FAILURE);
    }
    else
        fclose(f);
        
    printf("Ici %d, mon pid est %d, le pid de mon père est %d \n", 1, getpid(), getppid());
    forking(1); //Création de ses fils

    return 0;
}

