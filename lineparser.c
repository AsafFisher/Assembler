
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lineparser.h"
char* actions[] = {"mov","cmp","add","sub","no","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop"};
#define DATA ".data"
#define STRING ".string"
void parseLine(char* line){
  Symbole symbole;
  char *token;
  token = strtok(line, " ");
  if (token[strlen(token)-1]==':') {
    /* code */

    /*Remove the : from the Symbole*/
    token[strlen(token)-1] = '\0';
    symbole.name = malloc(strlen(token)+1);
    strcpy(symbole.name,token);
    printf("%s - Is a symbole.\n", symbole.name);
  }
  token = strtok(NULL, " ");
  if (!strcmp(token, DATA)) {
    /* code */
    printf("RealTok: %s\n", token);
  }
  if (!strcmp(token, STRING)) {
    /* code */
    printf("RealTok: %s\n", token);
  }


}
