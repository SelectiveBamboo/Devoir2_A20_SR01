#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILENAME "list_appli.txt"
#define CONFIG_SIZE (512)

typedef struct application {
  char *name;
  char *path;
  char **arguments;
} application;

int getNbApplications(char *str);
int countLinesInStr(char *str);
void allocatePointer(void *ptr, int size);
void trim (char *src, char *dest);
int parse_lineKeyValues(char *str, char *key, char *values);
int parse_list_appli();
void print_config();
int getNbApplications(char *str);

application *apps;

void printDebug(char *str, char *debug)
{
  printf("%s %s", str, debug);
}
int countLinesInStr(char *str) 
{
   int linesCount=0;

   while(*str++ != '\0') 
   {
      if(*str=='\n')
      { linesCount++; }
   }

   return linesCount;
}

//Allocate or reallocate the ptr of the given size (in byte)
void allocatePointer(void *ptr, int size)
{
  if (ptr == NULL)
  {
    ptr = malloc(size);
  }
  else
  {
    free(ptr);
    ptr = malloc(size);
  }
  
  if (ptr == NULL)
  {
    perror("Mémoire pleine\n");
    exit(EXIT_FAILURE);
  }
}

// trim the string, dest and src may be the same pointer, dest is however (re)allocateed to fit the trimmed string
void trim (char *src, char *dest)
{
  if (!src)
    return;

  int len = strlen(src);

  if (!len) {
    return;
  }
  char *ptr = src + len - 1;

  // remove trailing whitespace
  while (ptr > src && isspace(*ptr)) 
  { ptr--; } 

  ptr++;

  char *q;
  // remove leading whitespace
  for (q = src; (q < ptr && isspace(*q)); q++);

  allocatePointer(dest, (strlen(q)+1) * sizeof(char *));

  while (q < ptr)
  { *dest++ = *q++; }

  *dest = '\0';
  return;
}

/*Split the given line in str on '=' to parse it for a trimmed key having trimmed values associated.
If no '=' found to split the string on, no key is set and the whole line is considered to be values.
Returns 0 if the line was empty (or just whitespaces), 3 if only values has been populated, 2 if key and values were found, 
1 if only key has been populated.*/
int parse_lineKeyValues(char *str, char *key, char *values)
{
  char *token;
  int result;
  char *temp;

  trim(str, temp);
  if (strlen(temp) < 1)
  { return 0; } //The line is empty
  

  if ((token = strtok(str, "=")) == NULL)
  {
    allocatePointer(values, (strlen(temp)) * sizeof(char *));
    strcpy(values, temp);  //Copy str to values, but trimmed
    return 3;
  }
  else
  {
    trim(token, key); //copy the trimmed first part of str to key
    
    if ((token = strtok(str, "=")) == NULL)
    { return 1; }
    else
    {
      trim(token, values); //copy the trimmed last part of str to values
      return 2;
    }
  }
}



int parse_list_appli()
{
  FILE *f = fopen(FILENAME, "r");
  if (f == NULL)
  {
    perror("[CONFIG] Erreur - accès au fichier spécifié impossible");
    exit(EXIT_FAILURE);
  }
  
  //char buf[CONFIG_SIZE];
  char *line = NULL;
  size_t len = 0;
  ssize_t nRead;
  int line_number = 0;
  int nbApp;

  if (getline(&line, &len, f) > 0)
  {
    line_number++;
    
    int nbApp = getNbApplications(line);
    if (nbApp >= 0)
    {
      allocatePointer(apps, sizeof(application)*nbApp);
    }
    else
    {
      fprintf(stderr, "[CONFIG] Attendu: nombre_applications, ligne: %d\n", line_number);
      exit(EXIT_FAILURE);
    }    
  }
  else
  {
    perror("[CONFIG] Erreur lors de la lecture du fichier de configuration\n");
    exit(EXIT_FAILURE);
  }

  char *key;
  char *values;
  int numCurrentApp = 1;
  int appNbArguments = -1;
  int hasName, hasPath = 0;
  int readArguments;

  while ((nRead = getline(&line, &len, f)) != -1) 
  {
    line_number++;

    if ( numCurrentApp > nbApp )
    {
      printf("[CONFIG] Le nombre d'applications spécifié a été obtenu mais il reste des lignes, elles sont ignorées\n");
      break;
    }
    

    printf("Retrieved line of length %zu :\n", nRead);
    printf("%s", line);

    switch (parse_lineKeyValues(line, key, values))
    {
      //ligne vide
      case 0:
        if (appNbArguments >= 0 && readArguments == appNbArguments && (hasName & hasPath))
        {
          printf("L'app %d nommée %s a bien été ajoutée\n", numCurrentApp, apps[numCurrentApp].name);
          hasName, hasPath = 0;
          appNbArguments = -1;
          readArguments = 0;
          numCurrentApp++;
        }
        else
        {
          fprintf(stderr, "[CONFIG] Attendu: les clés name, path, nombre_arguments et arguments. Trouvé: ligne vide. ligne %d\n", line_number);
          exit(EXIT_FAILURE);
        }
        
        break;

      //Seule la clé a été trouvée
      case 1:
        if (strcmp(key, "arguments"))
        {
          if (appNbArguments >= 0)
          {
            for (readArguments = 0; readArguments < appNbArguments; readArguments++)
            {
              line_number++;
              if (getline(&line, &len, f) != -1)
              {
                trim(line, apps[numCurrentApp].arguments[readArguments]);
                readArguments++;
              }
              else
              {
                fprintf(stderr, "[CONFIG] Fin du fichier inattendue. ligne %d\n", line_number);
                exit(EXIT_FAILURE);
              }            
            }
          }
        }
        else
        {
          fprintf(stderr, "[CONFIG] Attendu: valeur de la clé %s. Trouvé: valeur vide. ligne %d\n", key, line_number);
          exit(EXIT_FAILURE);
        }
        
        break;

      //Clé et valeur ont été trouvées
      case 2:
        if (strcmp(key, "name"))
        {
          allocatePointer(apps[numCurrentApp-1].name, sizeof(char *)*strlen(values));
          strcpy(apps[numCurrentApp-1].name, values);
        }
        else if (strcmp(key,"path"))
        {
          allocatePointer(apps[numCurrentApp-1].path, sizeof(char *)*strlen(values));
          strcpy(apps[numCurrentApp-1].path, values);
        }
        else if (strcmp(key, "nombre_arguments"))
        {
          if (isdigit(values))
          {
            appNbArguments = atoi(values);

            if (appNbArguments >= 0)
            {
              allocatePointer(apps[numCurrentApp-1].arguments, appNbArguments*sizeof(char *));
            }
            else
            {
              fprintf(stderr, "[CONFIG] nombre_arguments n'a pas une valeur correcte, ligne %d\n", line_number);
              exit(EXIT_FAILURE);
            }
          }
          else
          {
            fprintf(stderr, "[CONFIG] nombre_arguments n'a pas un nombre en valeur, ligne %d\n", line_number);
            exit(EXIT_FAILURE);
          }          
        }
        else if (strcmp(key, "arguments"))
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
  }

  if (readArguments != appNbArguments)
  {
    fprintf(stderr, "[CONFIG] Problème au niveau des arguments de la dernière app, ligne %d\n", line_number);
    exit(EXIT_FAILURE);
  }
  

  if (line)
  { free(line); }
}

void print_config()
{
  for (int i = 0; i < (sizeof(apps)/sizeof(application *)); i++)
  {
    printf("L'application '%s' est au chemin '%s'. Elle prend comme arguments:\n", apps[i].name, apps[i].path);
    
    for (int j = 0; j < (sizeof(apps[i].arguments)/sizeof(char *)); j++)
    {
      printf("%s\n", apps[i].arguments[j]); 
    }
    printf("\n");
  }
  
}

int getNbApplications(char *str)
{
  int nbApplications = -1;
  char *token;

  //while ( (token = strtok(str, "=")) != NULL )
  token = strtok(str, "=");
  trim(token, token);

  if (strcmp(token, "nombre_applications"))
  {
    token = strtok(str, "=");
    trim(token, token);
    if ( isdigit(token) )
    { nbApplications = atoi(token); }
  }

  return nbApplications;
}


int main(int argc, char *argv[]) 
{
  if (argc != 1) 
  {
    fprintf(stderr, "Pas besoin d'arguments pour %s\n", argv[0]);
    return 1;
  }

  parse_list_appli();
  

 /* while (fgets(buf, sizeof(buf), f)) 
  {
    line_number++;
    err = parse_listAppli(buf, apps);
    if (err) fprintf(stderr, "error line %d: %d\n", line_number, err);
  }*/
  print_config();
  return 0;
}