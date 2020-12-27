#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_SIZE (512)

typedef struct application {
  char name[CONFIG_SIZE];
  char path[CONFIG_SIZE];
  char arguments[];
} application;


// trim the string
void trim (char *str)
{
  if (!str)
    return;

  int len = strlen(str);

  if (!len) {
    return;
  }
  char *ptr = str + len - 1;

  // remove trailing whitespace
  while (ptr > str && isspace(*ptr)) 
  { ptr--; } 

  ptr++;

  char *q;
  // remove leading whitespace
  for (q = str; (q < ptr && isspace (*q)); q++)
      ;

  while (q < ptr)
  { *str++ = *q++; }

  *str = '\0';
  return;
}

int parse_Listappli()
{

}

int parseNbApplications(char *str)
{
  int nbApplications = -1;
  char *token;

  while ( (token = strtok(str, "=")) != NULL )
  {


    if (strcmp(token, "nombre_applications"))
    {
      
    }
    
  }
  



  return nbApplications;
}


int main(int argc, char *argv[]) 
{
  if (argc != 2) 
  {
      fprintf(stderr, "Usage: %s CONFIG_FILE\n", argv[0]);
      return 1;
  }

  FILE *f = fopen(argv[1], "r");
  if (f == NULL)
  {
      perror("Erreur - accès au fichier spécifié impossible");
      exit(EXIT_FAILURE);
  }
  
  char buf[CONFIG_SIZE];
  application config[1];
  char *line = NULL;
  size_t len = 0;
  ssize_t nRead;
  while ((nRead = getline(&line, &len, f)) != -1) 
  {
      printf("Retrieved line of length %zu :\n", nRead);
      printf("%s", line);

      nRead++;

      
  }

  if (line)
    free(line); 
  

  while (fgets(buf, sizeof(buf), f)) 
  {
    line_number++;
    err = parse_listAppli(buf, config);
    if (err) fprintf(stderr, "error line %d: %d\n", line_number, err);
  }
  print_config(config);
  return 0;
}