#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <wait.h> 

#define FILENAME "list_appli.txt"
#define DEBUG 0

typedef struct application {
  char *name;
  char *path;
  int nargs;
  char **arguments;
} application;
int nbApp;
int *forkedApps;

int flag_shutdown = 0;

void forkingApps();
int startApp(application appToStart);
int getNbApplications(char *str);
int countLinesInStr(char *str);
void *allocatePointer(int size);
char *trim (char *src);
int parse_lineKeyValues(char *str, char *key, char *values);
void parse_list_appli();
void print_config();
int isNumber(char *str);
int getNbApplications(char *str);

application *apps;

void
printDebug(char *str, char *val, int debugLevel)
{
  if (debugLevel <= DEBUG)
    printf("%s {%s}\n", str, val);
}

int
countLinesInStr(char *str) 
{
  int linesCount=0;

  while(*str++ != '\0') 
  {
    if(*str=='\n')
      linesCount++;
  }

  return linesCount;
}

//Allocate the ptr of the given size (in byte)
void *
allocatePointer(int size)
{
  void *ptr = malloc(size);
  if (ptr == NULL)
  {
    perror("Mémoire pleine\n");
    exit(EXIT_FAILURE);
  }
  return ptr;
}

//Deallocate the ptr
void 
freePointer(void *ptr)
{
  if (ptr != NULL)
    free(ptr);
}


// trim a string
char *
trim(char * s)
{
  int l = strlen(s);

  while(isspace(s[l - 1])) --l;
  while(* s && isspace(* s)) ++s, --l;

  return strndup(s, l);
}

void
parse_list_appli()
{
  FILE *f = fopen(FILENAME, "r");
  char *line = NULL;
  size_t len = 0;
  int line_number = 0;
  int numCurrentApp = 0;
  int appNbArguments = -1;
  int hasName, hasPath = 0;
  int readArguments;

  if (f == NULL)
  {
    perror("[CONFIG] Erreur - accès au fichier spécifié impossible");
    exit(EXIT_FAILURE);
  }
 
 /* read first line to get the number of application defined */ 
  if (getline(&line, &len, f) > 0)
  {

    
    line_number++;
    
    nbApp = getNbApplications(line);
    if (nbApp >= 0)
    {
      /* allocate or storage of information for application to start */
      
      apps = allocatePointer(sizeof(application)*nbApp);
    }
    else
    {
      /* we do not have the required line, get out */
      fclose(f);
      fprintf(stderr, "[CONFIG] Attendu: nombre_applications, ligne: %d\n", line_number);
      exit(EXIT_FAILURE);
    }    
  }
  else
  {
    fclose(f);
    perror("[CONFIG] Erreur lors de la lecture du fichier de configuration\n");
    exit(EXIT_FAILURE);
  }

  // parsing du reste du fichier ligne par ligne
  while (getline(&line, &len, f) != -1) 
  {
    char *key;
    char *values;
    char *token;
    int status = 0;
    char *tmp;

    line_number++;

    // test si on excede le nombre d'appli declarees
    if ( numCurrentApp > nbApp )
    {
      printf("[CONFIG] Le nombre d'applications spécifié (%d) est suppérieur à celui attendu (%d), elles sont ignorées\n", numCurrentApp +1, nbApp);
      break;
    }

    // Extract key/value from delimiter = but trimmed and 
    // set the parsing status to specify the action to perform
    tmp = trim(line);
    //printDebug("read line:", tmp, 2);
    if (strlen(tmp) == 0)
    {
      status = 0;   //The line is empty
    }
    else if ((token = strtok(tmp, "=")) == NULL)
    {
      values = tmp;  //Copy all str to values, but trimmed
      status = 3;
    }
    else
    {
      key = trim(token); //copy the trimmed first part of str to key
      if ((token = strtok(NULL, "=")) == NULL)
        status = 1;
      else
      {
        values = trim(token); //copy the trimmed last part of str to values
        status = 2;
      }
    }

    switch (status)
    {
      //ligne vide
      case 0:
        break;

      //Seule la clé a été trouvée
      case 1:
        if (!strcmp(key, "arguments"))
        {
          if (appNbArguments >= 0)
          {
            for (readArguments = 0; readArguments < appNbArguments; readArguments++)
            {
              line_number++;
              if (getline(&line, &len, f) != -1)
              {
                apps[numCurrentApp].arguments[readArguments] = trim(line);
              }
              else
              {
                fclose(f);
                fprintf(stderr, "[CONFIG] Fin du fichier inattendue. ligne %d\n", line_number);
                exit(EXIT_FAILURE);
              }            
            }
          }
        }
        else
        {
          fclose(f);
          fprintf(stderr, "[CONFIG] Attendu: valeur de la clé %s. Trouvé: valeur vide. ligne %d\n", key, line_number);
          exit(EXIT_FAILURE);
        }
        
        break;

      //Clé et valeur ont été trouvées
      case 2:
        if (!strcmp(key, "name"))
        {
          apps[numCurrentApp].name = allocatePointer(sizeof(char)*(strlen(values)+1));
          strcpy(apps[numCurrentApp].name, values);
	        hasName = 1;
        }
        else if (!strcmp(key,"path"))
        {
          apps[numCurrentApp].path = allocatePointer(sizeof(char)*(strlen(values)+1));
          strcpy(apps[numCurrentApp].path, values);
	        hasPath = 1;
        }
        else if (!strcmp(key, "nombre_arguments"))
        {
          if (isNumber(values))
          {
            appNbArguments = atoi(values);
            apps[numCurrentApp].nargs = appNbArguments;

            if (appNbArguments >= 0)
            {
              apps[numCurrentApp].arguments = allocatePointer(appNbArguments*sizeof(char *));
            }
            else
            {
              fclose(f);
              fprintf(stderr, "[CONFIG] nombre_arguments n'a pas une valeur correcte, ligne %d\n", line_number);
              exit(EXIT_FAILURE);
            }
          }
          else
          {
            fclose(f);
            fprintf(stderr, "[CONFIG] nombre_arguments n'a pas un nombre en valeur, ligne %d\n", line_number);
            exit(EXIT_FAILURE);
          }          
        }
        else if (!strcmp(key, "arguments"))
        {
          fprintf(stderr, "[CONFIG] Les arguments doivent être placées sur une nouvelle ligne, ligne %d\n", line_number);
          exit(EXIT_FAILURE);
        }
        else
        {
          fprintf(stderr, "[CONFIG] Clé inattendue, ligne %d\n", line_number);
          exit(EXIT_FAILURE);
        }
        
        break;
      
      //La ligne ne contenait pas de '='
      case 3:
        fprintf(stdout, "[CONFIG] Ligne inattendue, elle est ignorée, ligne %d\n", line_number);
        break;
    }

    if (appNbArguments >= 0 && readArguments == appNbArguments && (hasName & hasPath))
        {
          printf("L'app %d nommée '%s' a bien été ajoutée\n", numCurrentApp +1, apps[numCurrentApp].name);
          hasName = hasPath = 0;
          appNbArguments = -1;
          readArguments = 0;
          numCurrentApp++;
        }
    freePointer(key);
    freePointer(values);
  }

  freePointer(line);
  fclose(f);
}

void 
print_config()
{
  printf("\n");
  for (int i = 0; i < nbApp; i++)
  {
    printf("L'application '%s' est au chemin '%s'. Elle prend comme arguments:\n", apps[i].name, apps[i].path);
    
    for (int j = 0; j < apps[i].nargs; j++)
    {
      printf("\t%s\n", apps[i].arguments[j]); 
    }
    printf("\n");
  }
  
}

int 
getNbApplications(char *str)
{
  int nbApplications = -1;
  char *token;

  token = strtok(str, "=");
  token = trim(token);

  if (!strcmp(token, "nombre_applications"))
  {
    token = strtok(NULL, "=");
    token = trim(token);
    if ( isNumber(token) )
      nbApplications = atoi(token);
  }

  return nbApplications;
}

int
isNumber(char *str)
{
  if (!str || strlen(str) == 0)
    return 0;

  for ( size_t i = 0; i < strlen(str); i++) 
  {
    if ( !isdigit(str[i]) )
      return 0;
  }

  return 1;
}

void
forkingApps()
{
  forkedApps = allocatePointer(nbApp*sizeof(int));
  for (int i = 0; i < nbApp; i++)
  {
    forkedApps[i] = fork();

    if (forkedApps[i] == -1)
    {
      perror("[AppManagement] Impossible de lancer les applications\n");
      exit(EXIT_FAILURE);
    }
    else if (forkedApps[i] == 0)
    {
      if (startApp(apps[i]) == -1)
      {
        fprintf(stderr, "[AppManagement] Erreur de l'application '%s'\n", apps[i].name);
        exit(EXIT_FAILURE);
      }
      else
      {
        exit(EXIT_SUCCESS);
      } 
    }       
  }
}

int 
numberIndexLookup (int number)
{
  int i = 0;
  
  while( i < nbApp )
  {
    if (forkedApps[i] == number)
      return i;

    i++;
  }
    

  return -1;
}


siginfo_t received_information;

static void 
sigchld_handler(int sig, siginfo_t *info, void *ucontext)
{
  
  //pid_t pid = wait(NULL);
  memmove(&received_information,
          info,
          sizeof(received_information)
         );

  pid_t pid = received_information.si_pid;


  int i = numberIndexLookup(pid);

  if(i != -1)
  {
    //printf("L'application %s s'est arrêtée.\n", apps[i].name);
    char *s = malloc(256*sizeof(char));
    //s = strcat(strcat("L'application ", apps[i].name), "s'est arrêtée");
    s = strcat(s, "L'application ");
    s = strcat(s, apps[i].name);
    s = strcat(s, " s'est arrêtée\n");
    write(1, s, strlen(s)+1);
  }
  else
    fprintf(stderr, "Inattendu, SIGCHLD envoyé par un processus inconnu: %d\n", pid);
  
  return;
}

static void 
sigusr1_handler(int sig, siginfo_t *info, void *ucontext)
{
  
  //pid_t pid = wait(NULL);

  pid_t pid;
  pid = info->si_pid;


  int i = numberIndexLookup(pid);

  if(i != -1)
  {
    if (!strcmp(apps[i].name, "power_manager"))
    {
      flag_shutdown = 1;
    }
    
  }

  return;
  
}

int
startApp(application appToStart)
{
  char **args = allocatePointer(sizeof(char *)*(appToStart.nargs + 2));

  args[0] = allocatePointer(sizeof(char)*(strlen(appToStart.name) + 1));
  strcpy(args[0], appToStart.path); 
  printDebug("bbbbbbb", args[0], 1 );
  
  for (int i = 1; i <= appToStart.nargs ; i++)
  {
    printDebug("aaaaaa", appToStart.arguments[i - 1], 1 );
    args[i] = allocatePointer(sizeof(char)*(strlen(appToStart.arguments[i - 1]) + 1));
    strcpy(args[i], appToStart.arguments[i - 1]);
  }

  args[appToStart.nargs+1] = NULL;

  printDebug("going to execute :", appToStart.name, 1);

  return execv(appToStart.path, args);
}


int 
main(int argc, char *argv[]) 
{
  if (argc != 1) 
  {
    fprintf(stderr, "Pas besoin d'arguments pour %s\n", argv[0]);
    return 1;
  }

  parse_list_appli();
  print_config();

  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = sigusr1_handler;

  struct sigaction sig;
  sig.sa_flags = SA_SIGINFO;
	sig.sa_sigaction = sigchld_handler;
  
  if( sigaction(SIGCHLD, &sig, NULL) != 0 || sigaction(SIGUSR1, &sa, NULL) != 0 )
  {
    perror("Erreur fatale dans la gestion des signaux");
    exit(EXIT_FAILURE);
  }

  forkingApps();

  int i = nbApp;
  while( i > 0)   //Attend la mort de ses fils
  {
    pid_t deadChild = waitpid(-1, NULL, WNOHANG);
    if (deadChild > 0)
    {
      int indexOfDeathChild = numberIndexLookup(deadChild);
     
      if (indexOfDeathChild != -1)
        forkedApps[indexOfDeathChild] = NULL;
      
      i--;
    }
      

    if (flag_shutdown)
    {
      for (size_t j = 0; j < nbApp; j++)
      {
        if (forkedApps[j] > 0)
          kill(forkedApps[j], SIGTERM); 
      }
      
    }
    
    usleep(10000);
  }
  

  return 0;
}

