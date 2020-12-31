#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <wait.h> 

#define FILENAME "list_appli.txt"
#define DEBUG 0

//Structure to store the config for apps handled
typedef struct application {
  char *name;
  char *path;
  int nargs;
  char **arguments;
} application;
int nbApp;  //number of app handled
int *forkedApps; //An array to store the PIDs of the app process. Index of process correpsond to index of their app in apps array

application *apps; //apps array, to store the configuration for each app to launch (Got from the config file)

int flag_shutdown = 0; //Whether it should shutdown or not, depending on SIGUSR1 received by power_manager

void forkingApps(); //Create the childs becoming the apps
int startApp(application appToStart); //Start the given appToSTart in the current process
int getNbApplications(char *str); //Get the number of application to generate upon a string
void *allocatePointer(int size); //Allocate the ptr of the given size (in byte)
void freePointer(void *ptr); //Deallocate the ptr
char *trim (char *src); // return the trimmed string s
void parse_list_appli(); //Parse the config file {FILENAME} to get the list of application
void print_config(); //Print the current list of application
int isNumber(char *str); //Tell whether the given string is a natural number
int numberIndexLookup (int number); //Get the index in the forkedApps array corresponding to the given number (used for PID)
static void sigchld_handler(int sig, siginfo_t *info, void *ucontext); //The hangler for SIGCHILD
static void sigusr1_handler(int sig, siginfo_t *info, void *ucontext); //The hangler for SIGCUSR1

void
printDebug(char *str, char *val, int debugLevel)
{
  if (debugLevel <= DEBUG)
    printf("%s {%s}\n", str, val);
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

// return the trimmed string s
char *
trim(char * s)
{
  int l = strlen(s);

  while(isspace(s[l - 1])) --l;
  while(* s && isspace(* s)) ++s, --l;

  return strndup(s, l);
}

//Parse the config file {FILENAME} to get the list of application
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
 
  //read first line to get the number of application defined
  if (getline(&line, &len, f) > 0)
  {
    line_number++;
    
    nbApp = getNbApplications(line);
    if (nbApp >= 0)
    {
      //allocate the storage of information for application to start 
      apps = allocatePointer(sizeof(application)*nbApp);
    }
    else
    {
      //do not have the required line, get out
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

  // Parse the rest of the file line by line
  while (getline(&line, &len, f) != -1) 
  {
    char *key;
    char *values;
    char *token;
    int status = 0;
    char *tmp;

    line_number++;

    // test whether exceeding the number of declared apps, but won't exit or what
    if ( numCurrentApp > nbApp )
    {
      printf("[CONFIG] Le nombre d'applications spécifié (%d) est suppérieur à celui attendu (%d), elles sont ignorées\n", numCurrentApp +1, nbApp);
      break;
    }

    // Extract key/value from delimiter = but trimmed and 
    // set the parsing status to specify the action to perform
    tmp = trim(line);

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
      //Empty line
      case 0:
        break;

      //the sole key has been found
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

      //key and values found
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
      
      //Line without '='
      case 3:
        fprintf(stdout, "[CONFIG] Ligne inattendue, elle est ignorée, ligne %d\n", line_number);
        break;
    }

    //if an appplication has been fully inquired, reset and let's do the next one
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

//Print the current list of application
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

//Get the number of application to generate upon a string 
int 
getNbApplications(char *str)
{
  int nbApplications = -1;
  char *token;

  //parse key / values
  token = strtok(str, "=");
  token = trim(token);

  //The key must be nombre_applications to get processed
  if (!strcmp(token, "nombre_applications"))
  {
    token = strtok(NULL, "=");
    token = trim(token);
    if ( isNumber(token) )
      nbApplications = atoi(token);
  }

  return nbApplications;
}

//Tell whether the given string is a natural number
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

//Create the childs bexoming the apps
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

  return;
}

//Get the index in the forkedApps array corresponding to the given number (used for PID)
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

//The hangler for SIGCHILD
static void 
sigchld_handler(int sig, siginfo_t *info, void *ucontext)
{
  
  //pid_t pid = wait(NULL);

  pid_t pid = info->si_pid;


  int i = numberIndexLookup(pid);

  if(i != -1)
  {
    //printf("L'application %s s'est arrêtée.\n", apps[i].name);
    char *s = allocatePointer((31+strlen(apps[i].name))*sizeof(char));
    strcpy(s, "");
    //s = strcat(strcat("L'application ", apps[i].name), "s'est arrêtée");
    s = strcat(s, "L'application ");
    s = strcat(s, apps[i].name);
    s = strcat(s, " s'est arrêtée\n");
    write(1, s, strlen(s)+1);
  }
  else
    write(2, "Inattendu, SIGCHLD envoyé par un processus inconnu\n", 53);
    //fprintf(stderr, "Inattendu, SIGCHLD envoyé par un processus inconnu: %d\n", pid);
  
  return;
}

//The handler for SIGUSR1
static void 
sigusr1_handler(int sig, siginfo_t *info, void *ucontext)
{
  //pid_t pid = wait(NULL);

  pid_t pid;
  pid = info->si_pid;


  int i = numberIndexLookup(pid);

  if(i != -1 && !strcmp(apps[i].name, "Power Manager"))
    flag_shutdown = 1;
  
  return;  
}

//Start the given appToSTart in the current process
int
startApp(application appToStart)
{
  char **args = allocatePointer(sizeof(char *)*(appToStart.nargs + 2));

  args[0] = allocatePointer(sizeof(char)*(strlen(appToStart.name) + 1));
  strcpy(args[0], appToStart.path); 

  
  for (int i = 1; i <= appToStart.nargs ; i++)
  {
    args[i] = allocatePointer(sizeof(char)*(strlen(appToStart.arguments[i - 1]) + 1));
    strcpy(args[i], appToStart.arguments[i - 1]);
  }

  args[appToStart.nargs+1] = NULL;

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
        forkedApps[indexOfDeathChild] = -1;
      
      i--;
    }

    if (flag_shutdown)
    {
      for (size_t j = 0; j < nbApp; j++)
      {
        if (forkedApps[j] > 0)
          kill(forkedApps[j], SIGTERM); 
      }
      printf("Il est toujours l'heure de faire une sieste\n");
      flag_shutdown = 0;
    }
    
    usleep(10000);
  }
  
  return 0;
}

