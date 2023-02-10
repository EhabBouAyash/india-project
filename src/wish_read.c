#include "wish.h"
int lastchar = 0;
int l =1;
char *wish_read_line(FILE *in) {
  


  //read to EOF or until line break 
  //if it reaches 80 characters and theres no line break, read to line break and discard everything
  //if it reaches before the 80, get it into the buffer and stop filling the buffer and print it. then read to  the next line break and discard everything else
  int linecount=1;
  if (in ==NULL){
    perror("File doesn't exist");
    exit(1);
  }

  int len = WISH_MAX_INPUT;
  size_t bufflen = len; 
  char * buff = malloc(sizeof(char)*len);
  buff[0] = '\0';
  while (getline(&buff, &bufflen, in)!=-1){
    if ((strchr(buff, '\n')) && (int)strlen(buff)<=WISH_MAX_INPUT){
      if (!isspace(buff[0])){
        printf("\nLine %d: \n%s",linecount++, buff);
      }
    }
    
    else if ((int)strlen(buff)<=WISH_MAX_INPUT){
      printf("\nLine %d: \n%s",linecount++, buff);
    }
    else{
      printf("wish: line too long");
      buff[0] = '\0';
    }
  }
  

  fclose(in);
  free(buff);



  //do the same for command line reading 
  
  return NULL;
}


//1 if missing
//0 if its there
int wish_read_config(char *fname, int ok_if_missing) {
  FILE * file;
  file = fopen(fname,"r");
  if (ok_if_missing==0){
    wish_read_line(file);
    return EXIT_SUCCESS;
  }
  else{
    return EXIT_FAILURE;
  }
  return 0;
}
