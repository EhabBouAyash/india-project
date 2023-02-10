#include "wish.h"
int main(int argc, char *argv[])
{
  // These two lines make the macOS C compiler happy.
  // Otherwise, it complains about unused parameters.
  
  argc = 0;
  argv = NULL;
  int ok_if_missing = 0; 
  char* home = getenv("HOME");
  char * path= strcat(home, "/wish.cfg");

  FILE * file = fopen(path,"r");
  if (!file){
    ok_if_missing =1;
    fclose(file);
  }
  printf("%s",home);
  printf("%s", WISH_DEFAULT_PROMPT);
  wish_read_config(path,ok_if_missing);
  return EXIT_SUCCESS;
}
