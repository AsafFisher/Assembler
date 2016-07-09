
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "lineparser.h"
char* actions[] = {"mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop"};
char* registers[] = {"‫‪r0","r1","r2","r3","r4","r5","r6","r7‬‬"};
#define ACTION_NUMBER 16
#define REGISTERS_NUMER 8
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
int updateCommandParamToMemory(Word command, char* token);
int evaluateTwoParamSize(Word *command,char* token);
int evaluateOneParamSize(Word *command,char* token);
int setUpCommandParams(Word *word,char* token);
int checkSrcParam(Word *command,char* token);






int parseLine(char* line){
  Symbole symbole;
  char *token;
  int hasSymbole;
  int actionID = 0;
  int wasFound;
  Word word;
  hasSymbole = false;





  token = strtok(line, " \n");
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
    token = strtok(NULL,"\t");
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
  {
    int iaction = 0;
    while(iaction<ACTION_NUMBER){
      if(!strcmp(actions[iaction],token)){
        wasFound = true;
        actionID = iaction;
        printf("%s Was found.\n", token);
        break;
      }
      iaction++;
    }
  }
  if (!wasFound) {
    /* code */
    printf("ERROR operator: '%s' Was not found! \n", token);
    return 0;
  }

  if (!checkSize(&codewords)) {
    /* ERROR ALLOCATING SPACE! */
    return 0;
  }
  word = createInstaceOfCommand(actionID);
  /*codewords.array[IC] = word;*/


/*FINISHED HERE! NEXT TIME FINISH FIRST WALKTHROUGH AND SECOND.*/
/*Check if parameter exist.*/

  {
    char* temp = strtok(NULL,"\n\t");
    if (temp==NULL&&word.command.grp!=NOP) {
      /*ERROR NO PARAMETERS*/
      printf("ERROR NO PARAMETER!\n");
      token =NULL;
    }
    token = temp;
  }
  if (token == NULL) {
    return 0;
  }
  if(!updateCommandParamToMemory(word,token)){
      /*ERROR: invalid parameter*/
    return 0;
  }






























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
  Word value;
  token = strtok(token,", \n");
  while(token!=NULL){
    long digitVal;
    int i = 0;

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
    /*Get int value of the token!*/
    digitVal = strtol(token, NULL,10);
    if (!checkSize(&datawords)) {
      /* ERROR ALLOCATING SPACE! */

      return 0;
    }
    printf("DATA value: %d\n",(int)digitVal );
    if(!setWordValue(&value,digitVal)){
      /*ERROR VALUE TOO BIG*/
      return 0;
    }
    datawords.array[DC] = value;
    printf("The value %d was added to DATAWORDS on place:%d \n",datawords.array[DC].word.cell,DC );
    token = strtok(NULL,", \n");
    DC++;
  }
  return 1;
}
int updateStringToMemory(char* token){
    /*TODO:If no "" mark
    PROBLEM: Cant detect char -> ""
    Check each letter untill you get to "
    */
    /*index of start of string*/
    int i = 0;
    int lindex;
    int found = 0;
    printf("TEST: %s\n",token );
    /*
    if(token[0]!='\"'){
      printf("ERROR missing first -> Cytation <-\n");
      return 0;
    }*/
    while (i<(strlen(token)-1)) {
      if (token[i]=='\"') {
        /*Find last quotes*/
        found = 1;
        break;
      }
      i++;
    }
    if (!found) {
      /*ERROR last quote not found!*/
      printf("ERROR missing first -> Cytation <-\n");
      return 0;
    }

    found = 0;
    i++;
    lindex = i;
    while (lindex<(strlen(token)-1)) {
      if (token[lindex]=='\"') {
        /*Find last quotes*/
        found = 1;
        break;
      }
      lindex++;
    }
    if (!found) {
      /*ERROR last quote not found!*/
      printf("ERROR missing last -> Cytation <-\n");
      return 0;
    }
    while (i<lindex) {
      /* ~ERROR SAGMENT FAULT~ Fixed */
      Word val;
      if (!checkSize(&datawords)) {
        /* ERROR ALLOCATING SPACE! */

        return 0;
      }
      if(!setWordValue(&val,token[i])){
        /*ERROR VALUE TOO BIG*/
        return 0;
      }
      datawords.array[DC] = val;
      printf("The value %c (%d) was added to DATAWORDS on place:%d \n",datawords.array[DC].word.cell,datawords.array[DC].word.cell,DC );
      i++;
      DC++;
    }

    return 1;
}
int updateCommandParamToMemory(Word command,char* token){
  int paramSize = 0;
  int error = 0;
  switch (command.command.grp) {
    case NOP:
    {
      int i = 0;
      while (i<strlen(token)) {
        if (token[i]==' '||token[i]=='\0'||token[i]=='\t') {
          /* code */
          i++;
        }
        error = 1;
        break;
      }
    }
    if (error) {
      /* code */
      printf("Invalid Token, no parameter required: '%s'\n",token);
      return 0;
    }

    return 1;
    case ONEOP:
    {
      int i = 0;
      while (i<strlen(token)) {
        if (token[i]==',') {
          /* ERROR TOO MANY PARAMS */
          printf("ERROR: \"%s\" is an iligal parameter for this type of command.\n",token );
          error = 1;
          break;
        }
        i++;
      }
    }
    if (error) {
      return 0;
    }
    if (!checkSize(&codewords)) {
      /* ERROR ALLOCATING SPACE! */

      return 0;
    }
    codewords.array[IC] = command;
    IC++;
    paramSize = setUpCommandParams(&codewords.array[IC-1],token);
    IC+=paramSize;



    printf("PARAMS: '%s'\n",token);
    return 1;
    case TWOP:
    {
      int found = 0;
      int i = 0;
      while (i<strlen(token)) {
        if(token[i]==','){
          found = 1;
          break;
        }
        i++;
      }
      if (!found) {
        printf("Missing parameter: %s\n",token );
        error = 1;
      }
    }
    if (error) {
      return 0;
    }
    printf("PARAMS: '%s'\n",token);
    if (!checkSize(&codewords)) {
      /* ERROR ALLOCATING SPACE! */

      return 0;
    }
    codewords.array[IC] = command;
    IC++;

    paramSize = setUpCommandParams(&codewords.array[IC-1],token);
    IC+=paramSize;









    return 1;
  }
  return 0;
}

int setUpCommandParams(Word* command,char *token){
  /*INSTANT_DYNAMIC_ADRESS_RESOLUTION structure...*/
	switch (command->command.opcode) {
		case cmp:

    /*FIRST PARAM:   -------------ERROR STRTOK NOT WOKRING!!!------------*/
    if ((token = strtok(token,","))==NULL) {
      printf("> ERROR: '%s'\n",token );
      return 0;
    }
    if(!checkSrcParam(command,token)){
      /*ERROR*/
      return 0;
    }

    /*-----------------------------------------------------*/


		break;
		case mov:
		case add:
		case sub:

		break;
		case nt:
		case clr:
		case inc:
		case dec:
		case jmp:
		case bne:
		case red:
		case jsr:

		break;
		case prn:
		break;
		case lea:
		break;
    default:
    printf("> ERROR LINE 476  \n");
    return 1;

	}
  return 1;

}

int checkSrcParam(Word *command,char* token){

  Word arg;
  int i = 0;
  int structureIndex = 0;
  char idarStructure[] = {'[','-',']'};
  int const NUMBEROFSTRUCTURE = 3;

  printf("Token: '%s'\n",token );
  /*Check if first parameter is a register...*/
  if(token[0] == '#'){
    command->command.srcar = INSTANT_ADDRESS_RESOLUTION;
    arg.pvalue.value = strtol(token,NULL, 10);
    codewords.array[IC] = arg;
    IC++;
    printf("Argument: '%u' CommandId: '%u' Command source address resolution: '%u' \n",arg.pvalue.value,codewords.array[IC-2].command.opcode,codewords.array[IC-2].command.srcar );
    return 1;
  }
  while (i < REGISTERS_NUMER) {
    if (!strcmp(registers[i],token)) {
      command->command.srcar = DIRECT_REGISTER_ADDRESS_RESOLUTION;
      arg.paddress.src = i;
      codewords.array[IC] = arg;
      IC++;
      return 1;
    }
  }
  i = 0;
  while (i<strlen(token)&&structureIndex<NUMBEROFSTRUCTURE) {
    if (token[i]==idarStructure[structureIndex]) {
      /* code */
      structureIndex++;
    }
    i++;
  }
  if (structureIndex==NUMBEROFSTRUCTURE) {
    command->command.srcar = INSTANT_DYNAMIC_ADDRESS_RESOLUTION;
    IC++;
    return 1;
  }
  command->command.srcar = DIRECT_ADDRESS_RESOLUTION;
  return 1;
}
