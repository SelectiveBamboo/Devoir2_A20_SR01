#include <stdio.h>
#include <error.h>
#include <unistd.h>
#include <sys/types.h>
#define FILENAME "orderedProc.temp"


int writeNumInFile(int n)
{
    FILE * fp;

    fp = fopen (FILENAME, "a");
    fprintf(fp, "\n%d", n);

    fclose(fp);

    return 0;
}

int countLinesInFile()
{
   FILE *fp;
   char ch;
   int linesCount=0;

   fp=fopen(FILENAME,"r");
   if(fp==NULL) {
      fprintf(stderr, "Le fichier \"%s\" n'existe pas !\n",FILENAME);
      return -1;
   }

   while((ch=fgetc(fp))!=EOF) 
   {
      if(ch=='\n')
         linesCount++;
   }
   fclose(fp);

   return linesCount;
}



int main(int argc, char const *argv[])
{
    /*begiining of initialisation*/
    int n = 1; //the process number in the specified tree order
    int numOfLines; 
    pid_t pid;

    FILE * fp;
    fp = fopen (FILENAME, "w");
    fprintf(fp, "%d", n); //each process will register its order in the tree on a new line
    fclose(fp);
    /*end of initialisation*/

    printf("Mon pid est %d, le pid de mon père est %d \n", getpid(), getppid());

    for (n = 2; n <= 5; n++)
    {
        pid = fork();
        if ( pid == -1)
        {
            perror("Impossible de créer le fils\n");
            n--;
        }
        else if (pid == 0)
        {
            printf("Mon pid est %d, le pid de mon père est %d \n", getpid(), getppid());
            writeNumInFile(n);
            break;
        }
    }
    
    printf("pid : %d  n:%d", pid, n);


    if (pid == 0)
    {
        switch (n)
        {
            case 2:
                while ((numOfLines = countLinesInFile()) != 5)
                {
                    if (!(numOfLines >= 0))
                    {
                        perror("Problème avec le fichier de suivi des processus\n");
                        printf("Il est conseillé de stopper le programme\n");
                        sleep(3);
                    }   
                }
                
                for (n = 6; n <= 8; n++)
                {
                    pid = fork();
                    if ( pid == -1)
                    {
                        perror("Impossible de créer le fils\n");
                        n--;
                    }
                    else if (pid == 0)
                    {
                        printf("Mon pid est %d, le pid de mon père est %d \n", getpid(), getppid());
                        writeNumInFile(n);
                        break;
                    }
                }

                break;

            case 3:
                while ((numOfLines = countLinesInFile()) != 8)
                {
                    if (!(numOfLines >= 0))
                    {
                        perror("Problème avec le fichier de suivi des processus\n");
                        printf("Il est conseillé de stopper le programme\n");
                        sleep(3);
                    }   
                }
                
                for (n = 9; n <= 10; n++)
                {
                    pid = fork();
                    if ( pid == -1)
                    {
                        perror("Impossible de créer le fils\n");
                        n--;
                    }
                    else if (pid == 0)
                    {
                        printf("Mon pid est %d, le pid de mon père est %d \n", getpid(), getppid());
                        writeNumInFile(n);
                        break;
                    }
                }

                case 4:
                    while ((numOfLines = countLinesInFile()) != 10)
                    {
                        if (!(numOfLines >= 0))
                        {
                            perror("Problème avec le fichier de suivi des processus\n");
                            printf("Il est conseillé de stopper le programme\n");
                            sleep(3);
                        }   
                    }
                    
                    for (n = 11; n <= 11; n++)
                    {
                        pid = fork();
                        if ( pid == -1)
                        {
                            perror("Impossible de créer le fils\n");
                            n--;
                        }
                        else if (pid == 0)
                        {
                            printf("Mon pid est %d, le pid de mon père est %d \n", getpid(), getppid());
                            writeNumInFile(n);
                            break;
                        }
                    }

            default:
                break;
        }

        if (pid == 0)
        {
            switch (n)
            {
                case 6:
                    while ((numOfLines = countLinesInFile()) != 11)
                    {
                        if (!(numOfLines >= 0))
                        {
                            perror("Problème avec le fichier de suivi des processus\n");
                            printf("Il est conseillé de stopper le programme\n");
                            sleep(3);
                        }   
                    }
                    
                    for (n = 12; n <= 13; n++)
                    {
                        pid = fork();
                        if ( pid == -1)
                        {
                            perror("Impossible de créer le fils\n");
                            n--;
                        }
                        else if (pid == 0)
                        {
                            printf("Mon pid est %d, le pid de mon père est %d \n", getpid(), getppid());
                            writeNumInFile(n);
                            break;
                        }
                    }

                    break;

                case 7:
                    while ((numOfLines = countLinesInFile()) != 13)
                    {
                        if (!(numOfLines >= 0))
                        {
                            perror("Problème avec le fichier de suivi des processus\n");
                            printf("Il est conseillé de stopper le programme\n");
                            sleep(3);
                        }   
                    }
                    
                    for (n = 14; n <= 14; n++)
                    {
                        pid = fork();
                        if ( pid == -1)
                        {
                            perror("Impossible de créer le fils\n");
                            n--;
                        }
                        else if (pid == 0)
                        {
                            printf("Mon pid est %d, le pid de mon père est %d \n", getpid(), getppid());
                            writeNumInFile(n);
                            break;
                        }
                    }

                    case 9:
                        while ((numOfLines = countLinesInFile()) != 14)
                        {
                            if (!(numOfLines >= 0))
                            {
                                perror("Problème avec le fichier de suivi des processus\n");
                                printf("Il est conseillé de stopper le programme\n");
                                sleep(3);
                            }   
                        }
                        
                        for (n = 15; n <= 15; n++)
                        {
                            pid = fork();
                            if ( pid == -1)
                            {
                                perror("Impossible de créer le fils\n");
                                n--;
                            }
                            else if (pid == 0)
                            {
                                printf("Mon pid est %d, le pid de mon père est %d \n", getpid(), getppid());
                                writeNumInFile(n);
                                break;
                            }
                        }

                default:
                    break;
            }

            if (pid == 0 && n == 12)
            {
                while ((numOfLines = countLinesInFile()) != 15)
                {
                    if (!(numOfLines >= 0))
                    {
                        perror("Problème avec le fichier de suivi des processus\n");
                        printf("Il est conseillé de stopper le programme\n");
                        sleep(3);
                    }   
                }
                
                for (n = 15; n <= 15; n++)
                {
                    pid = fork();
                    if ( pid == -1)
                    {
                        perror("Impossible de créer le fils\n");
                        n--;
                    }
                    else if (pid == 0)
                    {
                        printf("Mon pid est %d, le pid de mon père est %d \n", getpid(), getppid());
                        writeNumInFile(n);
                        break;
                    }
                }   
            }
        }            
    }
    
    return 0;
}
