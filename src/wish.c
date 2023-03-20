#include <limits.h>
#include "wish.h"
#include <signal.h>
#include <string.h>
#include <unistd.h>

int wish_exit = 0;

static void refuse_to_die(int sig)
{
  fputs("Type exit to exit the shell.\n", stderr);
}

void prevent_interruption() {
    struct sigaction sa;

    sa.sa_handler = refuse_to_die;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    
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
sprintf(path, "%s/%s", (home ? home : "."), WISH_CONFIG);
if (access(path, F_OK) != -1) {
  // Config file exists in HOME directory
  wish_read_config(path, 1);
} else {
  fprintf(stderr, "Could not find %s in HOME directory.\n", WISH_CONFIG);
}

  
  prevent_interruption();
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

    char *x = strdup(s);
    if (!x) {
        perror("super_strdup");
        exit(0);
    }
    return x;

}



void *super_malloc(size_t size) {

    void *x = malloc(size);
    if (!x) {
        perror("super_malloc");
        exit(0);
    }
    return x;

}



void *super_realloc(void *ptr, size_t size) {

    void *x = realloc(ptr, size);
    if (!x) {
        perror("super_realloc");
        abort();
    }
    return x;

}

