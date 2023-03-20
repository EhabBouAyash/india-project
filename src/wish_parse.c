#include <stdio.h>
#include "wish.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>


// https://en.wikipedia.org/wiki/Escape_sequences_in_C#Table_of_escape_sequences
char *wish_unquote(char * s) {
  int i, j;
  for (i = 0, j = 0; s[i]; ++i, ++j) {
    if(s[i] == '\\') {
      switch(s[++i]) {
      case 'a': s[j] = '\a'; break;
      case 'b': s[j] = '\b'; break;
      case 'f': s[j] = '\f'; break;
      case 'n': s[j] = '\n'; break;
      case 'r': s[j] = '\r'; break;
      case 't': s[j] = '\t'; break;
      case 'v': s[j] = '\v'; break;
      case '\\': s[j] = '\\'; break;
      case '\'': s[j] = '\''; break;
      case '"': s[j] = '"'; break;
      default:
	fprintf(stderr, "Invalid escape sequence\n");
	s[j] = 0;
	return s;
      }
    } else { //Simply copy
      s[j] = s[i];
    }
  }
  s[j] = 0; // NULL-terminate the string
  return s; 
}

// Do not modify this function
void yyerror(const char* s) {
  fprintf(stderr, "Parse error: %s\n", s);
}

char *wish_safe_getenv(char *s) {
  return s;
}
  // Find the first program on the command line
prog_t *last_exe(prog_t *exe) {
  while(exe->prev) exe = exe->prev;
  return exe;
}

// All these implementations are silly but they work
arglist_t add_to_arglist(arglist_t al, char *arg)
{
  al.size++;      // Increase argument array size
  al.args = super_realloc(al.args, sizeof(char*) * al.size); // Add storage
  al.args[al.size - 1] = arg;  // Add the last element
  return al;
}

arglist_t create_arglist(char *arg)
{
  arglist_t al;
  al.args = super_malloc(sizeof(char*)); // New argument array
  al.args[0] = arg; // Its first element
  al.size = 1;
  return al;
}

prog_t *create_program(arglist_t al)
{
  prog_t *p = super_malloc(sizeof(prog_t));
  p->args = al;
  p->redirection.in = p->redirection.out1 = p->redirection.out2 = NULL;
  p->prev = NULL;
  return p;
}

int spawn(prog_t *exe, int bgmode /* Disregard! */)
{
  pid_t pid; 
  int status=0;
  // Fork a child process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }
    else if (pid == 0) {  // Child process
        // Add another element to the array of arguments with realloc()
        exe->args.args = super_realloc(exe->args.args, (exe->args.size + 1) * sizeof(char *));
        exe->args.args[exe->args.size] = NULL;

        // Start new program as defined by exe->args.args[0]
        if (execvp(exe->args.args[0], exe->args.args) == -1) {
            perror("execvp");
            exit(0);
        }
    }
    else {  // Parent process
        // Free any previously allocated memory
        free_memory(exe, NULL);

        if (bgmode) {  // Background mode
            printf("Not waiting for my child :((");
        }
        else {  // Foreground mode
            waitpid(pid, &status, 0);
        }

        // Check for errors
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            return 0;
        }
        else {
            return 1;
        }
    }
	return status; 
}

void free_memory(prog_t *exe, prog_t *pipe)
{
  fputs("\nSYSTEM GHOST: I am a skeleton, just call me where necessary\n",
	stderr);
}
