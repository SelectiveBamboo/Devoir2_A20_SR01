#include <stdio.h>
#include <error.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

#define FILENAME "orderedProc.temp"


int writeNumInFile(int n)
{
    FILE * fp;

    fp = fopen (FILENAME, "a");
    fprintf(fp, "%d\n", n);

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


void forking(int n)
{
    int pid;

    int start;
    int end;
    int i;

    switch (n)
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
        
        default:
            start = 1;
            end = 0;
            break;
    }

    int numOfLines = countLinesInFile();
    
    while (numOfLines != (n-1))
    {
        if (!(numOfLines >= 0))
        {
            perror("Problème avec le fichier de suivi des processus\n");
            exit(EXIT_FAILURE);
        }
        numOfLines = countLinesInFile();
    }
    usleep(100000);
    writeNumInFile(n);
    printf("Process %d n'est plus bloqué, trouve %d lignes, attendu %d\n", n, numOfLines, n-1);
                
    for (i = start; i <= end; i++)
    {
        pid = fork();
        if ( pid == -1)
        {
            perror("Impossible de créer le fils\n");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            printf("Ici %d, mon pid est %d, le pid de mon père %d est %d \n", i, getpid(), n, getppid());
            forking(i);
            break;
        }
    }

    if(pid != 0)
    {
        //printf("Process %d, attente deces des fils\n", n);
        while( i > start)
        {
        
            if (waitpid(-1, NULL, WNOHANG) > 0)
            {
                //printf("Fils %d of %d die\n", p, n);
                i--;
            }
            usleep(100000);
        }
    }

    exit(0);
}

int main(int argc, char const *argv[])
{
    FILE * fp;
    fp = fopen (FILENAME, "w");     //erase an eventual already existing file
    fclose(fp);

    forking(1);

    return 0;
}


/*
int main(int argc, char const *argv[])
{
   
    int n = 1; //the process number in the specified tree order
    int numOfLines; 
    pid_t pid;
    int status;

    FILE * fp;
    fp = fopen (FILENAME, "w");
    fprintf(fp, "%d\n", n); //each process will register its order in the tree on a new line
    fclose(fp);
    

    printf("Ici %d, mon pid est %d, le pid de mon père est %d \n", n, getpid(), getppid());

    for (n = 2; n <= 5; n++)
    {
        pid = fork();
        if ( pid == -1)
        {
            perror("Impossible de créer le fils\n");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            printf("Ici %d, mon pid est %d, le pid de mon père est %d \n", n, getpid(), getppid());
            writeNumInFile(n);
            break;
        }
        else
        {
            waitpid(pid, &status, WNOHANG);
        }
        


    }

    if (pid == 0)
    {
        numOfLines = countLinesInFile();
        //printf("numOfLines : %d n : %d \n", numOfLines, n);

        switch (n)
        {
            case 2:
                while (numOfLines != 5)
                {
                    if (!(numOfLines > 0))
                    {
                        perror("Problème avec le fichier de suivi des processus\n");
                        exit(EXIT_FAILURE);
                    }
                    sleep(1);
                    numOfLines = countLinesInFile();
                }
                
                for (n = 6; n <= 8; n++)
                {
                    pid = fork();
                    if ( pid == -1)
                    {
                        perror("Impossible de créer le fils\n");
                        exit(EXIT_FAILURE);
                    }
                    else if (pid == 0)
                    {
                        printf("Ici %d, mon pid est %d, le pid de mon père est %d \n", n, getpid(), getppid());
                        writeNumInFile(n);
                        break;
                    }
                    else
                    {
                        waitpid(pid, &status, WNOHANG);
                    }
                }

                break;

            case 3:
                while (numOfLines != 8)
                {
                    if (!(numOfLines > 0))
                    {
                        perror("Problème avec le fichier de suivi des processus\n");
                        exit(EXIT_FAILURE);
                    }   
                    sleep(1);
                    numOfLines = countLinesInFile();
                }
                
                for (n = 9; n <= 10; n++)
                {
                    pid = fork();
                    if ( pid == -1)
                    {
                        perror("Impossible de créer le fils\n");
                        exit(EXIT_FAILURE);
                    }
                    else if (pid == 0)
                    {
                        printf("Ici %d, mon pid est %d, le pid de mon père est %d \n", n, getpid(), getppid());
                        writeNumInFile(n);
                        break;
                    }
                    else
                    {
                        waitpid(pid, &status, WNOHANG);
                    }
                }
                break;

            case 4:
                while (numOfLines != 10)
                {
                    if (!(numOfLines > 0))
                    {
                        perror("Problème avec le fichier de suivi des processus\n");
                        exit(EXIT_FAILURE);
                    } 
                    sleep(1);
                    numOfLines = countLinesInFile();  
                }
                
                for (n = 11; n <= 11; n++)
                {
                    pid = fork();
                    if ( pid == -1)
                    {
                        perror("Impossible de créer le fils\n");
                        exit(EXIT_FAILURE);
                    }
                    else if (pid == 0)
                    {
                        printf("Ici %d, mon pid est %d, le pid de mon père est %d \n", n, getpid(), getppid());
                        writeNumInFile(n);
                        break;
                    }
                    else
                    {
                        waitpid(pid, &status, WNOHANG);
                    }
                }
                break;

            default:
                break;
        }

        if (pid == 0)
        {
            numOfLines = countLinesInFile();

            switch (n)
            {
                case 6:
                    while (numOfLines != 11)
                    {
                        if (!(numOfLines > 0))
                        {
                            perror("Problème avec le fichier de suivi des processus\n");
                            exit(EXIT_FAILURE);
                        }
                        sleep(1);
                        numOfLines = countLinesInFile(); 
                    }
                    
                    for (n = 12; n <= 13; n++)
                    {
                        pid = fork();
                        if ( pid == -1)
                        {
                            perror("Impossible de créer le fils\n");
                            exit(EXIT_FAILURE);
                        }
                        else if (pid == 0)
                        {
                            printf("Ici %d, mon pid est %d, le pid de mon père est %d \n", n, getpid(), getppid());
                            writeNumInFile(n);
                            break;
                        }
                        else
                        {
                            waitpid(pid, &status, WNOHANG);
                        }
                    }

                    break;

                case 7:
                    while (numOfLines != 13)
                    {
                        if (!(numOfLines > 0))
                        {
                            perror("Problème avec le fichier de suivi des processus\n");
                            exit(EXIT_FAILURE);
                        }  
                        sleep(1);
                        numOfLines = countLinesInFile(); 
                    }
                    
                    for (n = 14; n <= 14; n++)
                    {
                        pid = fork();
                        if ( pid == -1)
                        {
                            perror("Impossible de créer le fils\n");
                            exit(EXIT_FAILURE);
                        }
                        else if (pid == 0)
                        {
                            printf("Ici %d, mon pid est %d, le pid de mon père est %d \n", n, getpid(), getppid());
                            writeNumInFile(n);
                            break;
                        }
                        else
                        {
                            waitpid(pid, &status, WNOHANG);
                        }
                    }
                    break;

                    case 9:
                        while (numOfLines != 14)
                        {
                            if (!(numOfLines > 0))
                            {
                                perror("Problème avec le fichier de suivi des processus\n");
                                exit(EXIT_FAILURE);
                            }   
                            sleep(1);
                            numOfLines = countLinesInFile();
                        }
                        
                        for (n = 15; n <= 15; n++)
                        {
                            pid = fork();
                            if ( pid == -1)
                            {
                                perror("Impossible de créer le fils\n");
                                exit(EXIT_FAILURE);
                            }
                            else if (pid == 0)
                            {
                                printf("Ici %d, mon pid est %d, le pid de mon père est %d \n", n, getpid(), getppid());
                                writeNumInFile(n);
                                break;
                            }
                            else
                            {
                                waitpid(pid, &status, WNOHANG);
                            }
                        }

                default:
                    break;
            }

            if (pid == 0 && n == 12)
            {
                while (numOfLines != 15)
                {
                    if (!(numOfLines > 0))
                    {
                        perror("Problème avec le fichier de suivi des processus\n");
                        exit(EXIT_FAILURE);
                    } 
                    sleep(1);
                    numOfLines = countLinesInFile();
                }
                
                for (n = 16; n <= 16; n++)
                {
                    pid = fork();
                    if ( pid == -1)
                    {
                        perror("Impossible de créer le fils\n");
                        exit(EXIT_FAILURE);
                    }
                    else if (pid == 0)
                    {
                        printf("Ici %d, mon pid est %d, le pid de mon père est %d \n", n, getpid(), getppid());
                        writeNumInFile(n);
                        break;
                    }
                    else
                    {
                        waitpid(pid, &status, WNOHANG);
                    }                    
                }   
            }
        }            
    }
    
    exit(0);
}
*/
