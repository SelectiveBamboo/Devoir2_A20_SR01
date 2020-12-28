#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILENAME "list_appli.txt"
#define CONFIG_SIZE (512)

typedef struct application {
  char name[CONFIG_SIZE];
  char path[CONFIG_SIZE];
  char arguments[];
} application;

application *apps;


int countLinesInStr(char *str) 
{
   int linesCount=0;

   while((str++ != '\0') 
   {
      if(*str=='\n')
      { linesCount++; }
   }

   return linesCount;
}

// trim the string
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

  while (q < ptr)
  { *dest++ = *q++; }

  *dest = '\0';
  return;
}

/*Split the given line in str on '=' to parse it for a trimmed key having trimmed values associated.
If no '=' found to split the string on, no key is set and the whole line is considered to be values.
Returns 0 if the line was empty (or just whitespaces), 1 if only values has been populated, 2 if key and values were found, 
3 if only key has been populated.*/
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
    strcpy(temp, values);  //Copy str to values, but trimmed
    return 1;
  }
  else
  {
    trim(token, key); //copy the trimmed first part of str to key
    
    if ((token = strtok(str, "=")) == NULL)
    { return 3; }
    else
    {
      trim(token, values); //copy the trimmed last part of str to values
      return 2;
    }
  }
}



int parse_list_appli(application *apps)
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

  if (getline(&line, &len, f) > 0)
  {
    line_number++;
    
    int nbApp = getNbApplications(line);
    if (nbApp >= 0)
    {
      apps = malloc( sizeof(application)*nbApp );
    }
    else
    {
      fprintf(stderr, "[CONFIG] Erreur, ligne: %s\n", line_number);
      printf("[CONFIG] Ligne: %s \n Attendu: nombre_applications\n", line_number);
      exit(EXIT_FAILURE);
    }    
  }
  else
  {
    perror("[CONFIG]Erreur inconnue lors de la lecture du fichier de configuration\n");
    exit(EXIT_FAILURE);
  }

  char *key;
  char *values;
  int numCurrentApp = 1;
  int appNbArguments = -1;

  while ((nRead = getline(&line, &len, f)) != -1) 
  {
    line_number++;

    if ( numCurrentApp > sizeof(apps) )
    {
      printf("[CONFIG] Le nombre d'applications spécifié a été obtenu mais il reste des lignes, elles sont ignorées\n");
      break;
    }
    

    printf("Retrieved line of length %zu :\n", nRead);
    printf("%s", line);

    switch (parse_lineKeyValues(line, key, values))
    {
      case 0:
        /* code */
        break;

      case 1:
        /* code */
        break;

      case 2:
        /* code */
        break;
      
      case 3:
        /* code */
        break;
      
      default:
        break;
    }
  }

  if (line)
  { free(line); }
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

  
  

  while (fgets(buf, sizeof(buf), f)) 
  {
    line_number++;
    err = parse_listAppli(buf, apps);
    if (err) fprintf(stderr, "error line %d: %d\n", line_number, err);
  }
  print_config(apps);
  return 0;
}