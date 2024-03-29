#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "wish.h"
#include <string.h>

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

char *wish_safe_getenv(char *name) {
  static char *empty = "";
  char *var = getenv(name);  
  if (var)
    return var;
  else
    return empty;
}

void wish_assign(char *name, char *value) {
  if(setenv(name, value, 1))
    perror(name);
  free(name);
  free(value);
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

int handle_child(pid_t pid, int bgmode)
{
  if(!bgmode) {
    int wstatus;
    if (-1 == waitpid(pid, &wstatus, 0)) {
      perror("waitpid");
      return 1;
    }
    
    if (WIFEXITED(wstatus)) {
      char value[16];
      snprintf(value, sizeof value, "%d", WEXITSTATUS(wstatus));
      if(setenv("_", value, 1))
	return 1;
    } else
      return 1;    
  }
  return 0;
}

// "Rename" fule descriptor "old" to "new," if necessary. After the
// execution of this function a program that "believes" that it uses
// the "old" descriptor (e.g., stdout #1 for output) will be actually
// using the "new" descriprot (e.g., an outgoinf pipe).  This
// functions terminates the process of error and should not be used in
// the parent, only in a child.
static void dup_me (int new, int old) {
  if (new != old && -1 == dup2(new, old)) {
    perror("dup2");
    exit(1);
  }
}

// Execute the program defined in "exe"
static void start(prog_t *exe) {
  arglist_t args = exe->args;
  args.args = super_realloc(args.args, sizeof(char*) * (args.size + 1));
  args.args[args.size] = (char*)NULL;
  execvp(args.args[0], args.args);
  perror(args.args[0]);
}
// Find the number of programs on the command line
static size_t cmd_length(prog_t *exe) {
  int count = 0;
  while(exe) {
    exe = exe->prev;
    count++;
  }
  return count;
}

int spawn(prog_t *exe, int bgmode) {
  int num_programs = 0;
  prog_t *p = exe;
  while (p != NULL) { // Count the number of programs
    num_programs++;
    p = p->prev;
  }

  int pipefd[num_programs - 1][2]; // One pipe per program pair
  pid_t pid[num_programs]; // One PID per program

  // Create pipes
  p = exe;
  for (int i = num_programs - 1; i > 0; i--) {
    if (pipe(pipefd[i-1]) == -1) {
      perror("pipe");
      return 1;
    }
    p = p->prev;
  }

  // Spawn child processes
  p = exe;
  for (int i = num_programs - 1; i > -1; i--) {
    switch(pid[i] = fork()) {
      case -1:
        perror("fork");
        return 1;
      
      case 0: // Child
        if (i == num_programs - 1) { //we are the last program
          dup2(pipefd[i-1][0], STDIN_FILENO); // Connect stdin to read end of previous pipe
        }
        else if (i != 0 && i<num_programs-1) { // Not first or last program
          dup2(pipefd[i][1], STDOUT_FILENO); // Connect stdout to write end of pipe
          dup2(pipefd[i-1][0], STDIN_FILENO); // Connect stdin to read end of previous pipe
        }

        else { // First program
          dup2(pipefd[i][1], STDOUT_FILENO); // Connect stdout to write end of pipe
        }
		for (int j = 0; j < num_programs - 1; j++) { // Close all pipe ends in child processes
          close(pipefd[j][0]);
          close(pipefd[j][1]);
        } 
        start(p);
        _exit(EXIT_FAILURE); // Do NOT use exit()!
      
      default: // Parent
        break;
    }
    p = p->prev;
  }

  // Close all pipe ends
  for (int i = 0; i < num_programs - 1; i++) {
    close(pipefd[i][0]);
    close(pipefd[i][1]);
  }
  // Wait for children to terminate if not in background mode
  if (!bgmode) {
    for (int i = 0; i < num_programs; i++) {
      waitpid(pid[i], NULL, 0);
    }
  }

  free_memory(exe);
  return 0;
}
void free_memory(prog_t *exe)
{
  if (exe==NULL){return ;}
  free_memory(exe->prev);
  for (int i=0; i<exe->args.size;i++){
  	free(exe->args.args[i]);
  }
  if (exe->redirection.out1 && exe->redirection.out2 && exe->redirection.in){
  	free(exe->redirection.in);
  	free(exe->redirection.out1);
  	free(exe->redirection.out2);
  }
  
}


