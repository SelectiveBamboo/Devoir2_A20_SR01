#include <stdio.h>
#include <error.h>
#include <unistd.h>
#include <sys/types.h>
#define FILENAME "Prog1.temp"


int writeNumInFile(int n)
{
    FILE * fp;

    fp = fopen (FILENAME, "a");
    fprintf(fp, "\n %d", n);

    fclose(fp);

    return(0);
}

int countLinesInFile()
{
   FILE *fp;
   char ch;
   int linesCount=0;

   fp=fopen(FILENAME,"r");
   if(fp==NULL) {
      fprintf(stderr, "File \"%s\" does not exist !\n",FILENAME);
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
    printf("Mon pid est %d, le pid de mon père est %d \n", getpid(), getppid());
    int i = 1;
    pid_t pid;

    writeNumInFile(i);

    for (i = 2; i < 5; i++)
    {
        pid = fork();
        if ( pid == -1)
        {
            perror("Impossible de créer le fils\n");
        }
        else if (pid == 0)
        {
            printf("Mon pid est %d, le pid de mon père est %d \n", getpid(), getppid());
            break;
        }
    }
    

        

    
    
    return 0;
}
