#include <stdio.h>
#include "wish.h"

char *wish_unquote(char *s) {

    int j=0; 
    char result; 
    char* copy = malloc(2*(int)strlen(s));
    for (int i=0; i<strlen(s);i++){
      if (s[i]=='\\'){
        char temp = s[i+1];
        switch (temp){
          case 'n':
            result = '\n';
            break;
          case 't':
            result = '\t';
            break;
          case 'v':
            result = '\v';
            break;
          case 'r':
            result = '\r';
            break;
          case 'a':
            result = '\a';
            break;
          case 'e':
            result = '\e';
            break;
          case 'f':
            result = '\f';
            break;
          default:
            result = temp;
        }
        copy[j++] = result; 
        i++;
      }
      else{
        copy[j++] = s[i];
      }
      
    }
    printf("%s\n",copy);
    s[0] = '\0';
    strcpy(s,copy);
    return s; //MODIFY 
}

// Do not modify this function
void yyerror(const char* s) {
  fprintf(stderr, "Parse error: %s\n", s);
}

char *wish_safe_getenv(char *s) {
  return s;
}
