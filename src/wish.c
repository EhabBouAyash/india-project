#include <limits.h>
#include "wish.h"




int main(int argc, char *argv[])
{
  // These two lines make the macOS C compiler happy.
  // Otherwise, it complains about unused parameters.
  (void)argc;
  (void)argv;

  char path[PATH_MAX];
  char *home = getenv("HOME");
#ifdef DEBUG
  home = "."; // So that you could place the config into the CWD
#endif
  sprintf(path, "%s/%s", (home ? home : "."), WISH_CONFIG);
  wish_read_config(path, 1);
  
  // This is just a skeleton for your convenience
  fputs(WISH_DEFAULT_PROMPT, stdout);
  char *line = wish_read_line(stdin);
  if(line)
    free(line);
  

  //testing wish_parse.c
  char *s = malloc(WISH_MAX_INPUT);
  strcpy(s,"This is a string\\n\\a this is another\\n this yet another\\n");

  s = wish_unquote(s);
  return EXIT_SUCCESS;
}
