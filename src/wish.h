#ifndef WISH_H
#define WISH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG
#define WISH_MAX_INPUT 80 // really modest :)
#define WISH_DEFAULT_PROMPT "> "
#define WISH_CONFIG "wish.cfg"

int wish_exit; 
char *wish_read_line(FILE *in);
int wish_read_config(char *fname, int ok_if_missing);
char *wish_safe_getenv(char *s);
char *wish_unquote(char * s);
int insert_into_hashtable(char key, char* value);
int wish_parse_command(char*s);
int hash(char c);
#endif
