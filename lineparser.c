
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "lineparser.h"
char* actions[] = {"mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop"};
#define ACTION_NUMBER 16
#define DATA ".data"
#define STRING ".string"
#define EXTERN ".extern"
#define ENTRY ".entry"
enum{false = 0,true};
Words codewords;
Words datawords;
Symboles symboles;
int addSymbole(Symbole symbole);
int updateDataToMemory(char* token);
int updateStringToMemory(char* token);
int parseLine(char* line){
  Symbole symbole;
  char *token;
  int hasSymbole;
  int iaction = 0;
  int wasFound;

  Word word;
  hasSymbole = false;

  token = strtok(line, " ");
  if (token[strlen(token)-1]==':') {
    /* code */

    /*Remove the : from the Symbole*/
    hasSymbole = true;
    token[strlen(token)-1] = '\0';
    symbole.name = malloc(strlen(token)+1);
    strcpy(symbole.name,token);
    token = strtok(NULL," \n");
  }

  /*-------------------Detect DATA numbers and store them + detect data Symboles-------------------------*/
  if (!strcmp(token, DATA)) {
    /* code */
    if (hasSymbole) {
      /* Symbole exist: */
      symbole.type = INSTRUCTION;
      symbole.address.word.cell = DC;
      symbole.isExternal = 0;
      if(!addSymbole(symbole)){
        /*ERROR!*/
        return 0;
      }
    }
    token = strtok(NULL,"\t");
    /*TODO: Built update memory.
    Problem: No way to detect ints.
    */
    if(!updateDataToMemory(token)){
      /*Error Saving data.*/
      return 0;
    }
    return 1;

  }
  /*--------------------------------------------------------------------------------*/

  /*Detect String data store them + Detect lables/Symboles.*/
  if (!strcmp(token, STRING)) {
    /* code */
    if (hasSymbole) {
      symbole.type = INSTRUCTION;
      symbole.address.word.cell = DC;
      symbole.isExternal = 0;
      if(!addSymbole(symbole)){
        /*ERROR!*/
        return 0;
      }
    }

    /*TODO: Built update memory.
    Problem: No way to detect ints.
    */
    printf("%d\n", token[11] );/*-30  -128 -99----------------------------TODO: What are these values.*/
    if(!updateStringToMemory(token)){
      /*Error Saving data.*/
      return 0;
    }
    return 1;

  }

  if (!strcmp(token, EXTERN)) {
    if (hasSymbole) {
      symbole.type = INSTRUCTION;
      symbole.address.word.cell = 0;
      symbole.isExternal = 1;
      addSymbole(symbole);
      return 1;
    }

  }
  if (!strcmp(token, ENTRY)) {
    /* TODO: NO Idea what to do.*/
    return 1;
  }

  if(hasSymbole){
    symbole.type = ACTION;
    symbole.address.word.cell = IC;
    symbole.isExternal = true;
    addSymbole(symbole);
  }

  wasFound = false;
  while(iaction<ACTION_NUMBER){
    if(!strcmp(actions[iaction],token)){
      wasFound = true;
      printf("%s Was found.\n", token);
    }
    iaction++;
  }
  if (!wasFound) {
    /* code */
    printf("ERROR operator: '%s' Was not found! \n", token);
    return 0;
  }

  if (codewords.size<=codewords.numberOfWords) {
    /* code */
    printf("> RESIZE WORDS ARRAY!\n");
    codewords.size += 10;
    /*WARNING: DONT REALLOC DIRECTLY TO SAME PARAM!-----------------------------------------*/
    codewords.array = realloc(codewords.array, sizeof(Word)*codewords.size);
  }
  word = createInstaceOfCommand(iaction);
  codewords.array[IC] = word;

/*FINISHED HERE! NEXT TIME FINISH FIRST WALKTHROUGH AND SECOND.*/
































return 1;

}
int addSymbole(Symbole symbole){
  int i = 0;
  /*if(symboles.size == 0){
    symboles.size += 10;
    symboles.symboles = malloc(symboles.size);
  }*/
  if (symboles.size <= symboles.numberOfSymboles) {
    /* code */
    symboles.size += 10;
    if((symboles.array = realloc(symboles.array,sizeof(symbole)*symboles.size))==NULL){
      printf("ERROR!\n");
    }
  }

  while (i<symboles.numberOfSymboles) {
    /* code */
    if(!strcmp(symboles.array[i].name,symbole.name)){
    /*-------------------ERROR: Multiple declearation!----------------------*/
      printf("ERROR: duplicate declearation: %s\n",symbole.name);
      return 0;
    }
    i++;
  }
  symboles.array[symboles.numberOfSymboles] = symbole;
  printf("numberOfSymboles: %d\n",symboles.numberOfSymboles);
  printf("Added: '%s' to Symboles\n",symboles.array[symboles.numberOfSymboles].name);
  symboles.numberOfSymboles++;
  return 1;
}

int updateDataToMemory(char* token){
  /*TODO: DETECT NUMBERERS AND */

  token = strtok(token,", \n");
  while(token!=NULL){
    long digitVal;
    int i = 0;
    printf("Num: %s\n",token );

    /*TODO:
    1. If Chars in mid mark error.
    2. Convert string to number.
    */

    while (i<strlen(token)) {
      /* code */
      if (!(isdigit(token[i]))&&!(token[i]=='-')&&!(token[i]=='+')) {
        /* code */
        printf("ERROR: '%s' is invalid value!\n",token );
        return 0;
      }
      i++;
    }
    digitVal = strtol(token, NULL,10);

    printf("DATA value: %d\n",(int)digitVal );
    token = strtok(NULL,", \n");
    DC+=1;
  }
  return 1;
}
int updateStringToMemory(char* token){
    /*TODO:If no "" mark
    PROBLEM: Cant detect char -> ""

    */
    printf("%c\n",token[1] );
    if(token[0]!='"'){
      printf("ERROR missing first -> Cytation <-\n");
      return 0;
    }
    if(token[strlen(token)-1]!='"'){
      printf("ERROR missing last -> Cytation <-\n");
      return 0;
    }

    printf("STRING: %s\n",token );
    token = strtok(NULL,",");
    return 1;
}
