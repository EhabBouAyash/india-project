#include "wish.h"
int main(int argc, char *argv[])
{
  // These two lines make the macOS C compiler happy.
  // Otherwise, it complains about unused parameters.
  
  argc = 0;
  argv = NULL;
  int ok_if_missing = 0; 
  char* home = getenv("HOME");
  // DZ: You cannot do this: there is not enough space
  // DZ:  char buffer[strlen(home) + sizeof("/wish.cfg") + 1];
  // DZ:  strcpy(buffer, home);
  // DZ:  strcat(buffer, "/wish.cfg");
  char * path= buffer;//strcat(home, "/wish.cfg");

  // DZ: This makes no sense
  FILE * file = fopen(path,"r");
  if (!file){
    ok_if_missing =1;
    // DZ: You cannot close a file that wasn't opened
    //    fclose(file);
  }
  // DZ: ???
  printf("%s",home);
  printf("%s", WISH_DEFAULT_PROMPT);
  wish_read_config(path,ok_if_missing);
  return EXIT_SUCCESS;
}
