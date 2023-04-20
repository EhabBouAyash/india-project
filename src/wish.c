#include <limits.h>
#include <signal.h>
#include <string.h>
#include "wish.h"
#include <unistd.h>

int wish_exit = 0;

static void refuse_to_die(int sig)
{
  (void)sig; // To make macOS compiler happy
  fputs("Type exit to exit the shell.\n", stderr);
}

static void prevent_interruption() {
  const struct sigaction sa = {.sa_handler = refuse_to_die };
  if(sigaction(SIGINT, &sa, NULL))
    perror("sigaction");
}

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
  if (!home){
  	fprintf(stderr, "home environement variable not set");
  	exit(1);
  }
  
  sprintf(path, "%s/%s", (home ? home : "."), WISH_CONFIG);
  if (access(path,F_OK)==0){
  	wish_read_config(path, 1);	
  }
  
  
  
  char* default_prompt = WISH_DEFAULT_PROMPT;
  setenv("SHELL", argv[0], 1);
  setenv("PS1", WISH_DEFAULT_PROMPT, 1);
  
  
  prevent_interruption();
  for (int i=1; i< argc;i++){
  	if (access(argv[i],F_OK)==-1){
  		fprintf(stderr, "File %s doesn't exist. ", argv[i]);
  		exit(1);
  	}
  	wish_read_config(argv[i],0);
  }
  while(!wish_exit) {
    fputs(WISH_DEFAULT_PROMPT, stdout);
    char *line = wish_read_line(stdin);
    if(line) {
      
      wish_parse_command(line);
      free(line);
    }
  }
  
  return EXIT_SUCCESS;
}

char *super_strdup(const char *s) {
  char *s_dup = strdup(s);
  if(!s_dup) abort();
  return s_dup;
}

void *super_malloc(size_t size) {
  void *s = malloc(size);
  if(!s) abort();
  return s;
}

void *super_realloc(void *ptr, size_t size) {
  void *s = realloc(ptr, size);
  if(!s) abort();
  return s;
}

