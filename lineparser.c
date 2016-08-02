
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "lineparser.h"
char* actions[] = {"mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop"};
char* registers[] = {"r0","r1","r2","r3","r4","r5","r6","r7‬‬"};
#define ACTION_NUMBER 16
#define REGISTERS_NUMER 8
#define SOURCE 0
#define DESTINATION 1
#define kMAX_SYMBOL_NAME_SIZE 20
#define DATA ".data"
#define STRING ".string"
#define EXTERN ".extern"
#define ENTRY ".entry"
enum{false = 0,true};
Words codewords;
Words datawords;
Symbols symbols;
Undefineds unds;
int lineNumber;
int addSymbol(Symbol symbol);
int updateDataToMemory(char* token);
int updateStringToMemory(char* token);
int updateCommandParamToMemory(Word command, char* token);
int evaluateTwoParamSize(Word *command,char* token);
int evaluateOneParamSize(Word *command,char* token);
int setUpCommandParams(Word *word,char* token);
int checkSrcParam(Word *command,char* token);
/*Check if param is INSTANT_ADDRESS_RESOLUTION, create a Word and save space in memory.
* location is the type of the assignment (SOURCE OR DESTINATION).
*/
int isParamIAR(Word *command,char* token, int location);
/*Check if param is DIRECT_REGISTER_ADDRESS_RESOLUTION, create a Word and save space in memory.*/
int isParamDRAR(Word *command,char* token, int location);
/*Check if param is INSTANT_DYNAMIC_ADDRESS_RESOLUTION, create a Word and save space in memory.*/
int isParamIDAR(Word *command,char* token, int location);
int addUndefined(char *token);
int checkSymbolSize();
int checkUndSize(Undefined *und);
int checkUndsSize(Undefineds *und);




int parseLine(char* buff,int number){
  Symbol symbol;
  char *token;
  int hasSymbol;
  int actionID = 0;
  int wasFound;
  Word word;
  hasSymbol = false;
  lineNumber = number;

  if (!checkSize(&codewords)) {
    /* ERROR ALLOCATING SPACE! */
    return 0;
  }
  if (!checkSize(&datawords)) {
    /* ERROR ALLOCATING SPACE! */
    return 0;
  }

if(!checkUndsSize(&unds)){
  /* ERROR ALLOCATING SPACE! */
  return 0;
}



    if(!(token = strtok(buff, " \n"))){
        return 1;
    }
  if (token[strlen(token)-1]==':') {
    /* code */

    /*Remove the : from the Symbole*/
    hasSymbol = true;
    token[strlen(token)-1] = '\0';
    symbol.name = malloc(strlen(token)+1);
    strcpy(symbol.name,token);
    token = strtok(NULL," \n");
  }

  /*-------------------Detect DATA numbers and store them + detect data Symbols-------------------------*/
  if (!strcmp(token, DATA)) {
    /* code */
    if (hasSymbol) {
      /* Symbole exist: */
      symbol.type = INSTRUCTION;
      symbol.address.fullword.cell = (unsigned int)datawords.numberOfWords;
      symbol.isExternal = 0;
      if(!addSymbol(symbol)){
        /*ERROR!*/
        return 0;
      }
    }
    token = strtok(NULL,"\t");

    if(!updateDataToMemory(token)){
      /*Error Saving data.*/
      return 0;
    }
    return 1;

  }
  /*--------------------------------------------------------------------------------*/

  /*Detect String data store them + Detect lables/Symbols.*/
  if (!strcmp(token, STRING)) {
    /* code */
    if (hasSymbol) {
      symbol.type = INSTRUCTION;
      symbol.address.fullword.cell = (unsigned int)datawords.numberOfWords;
      symbol.isExternal = 0;
      if(!addSymbol(symbol)){
        /*ERROR!*/
        return 0;
      }
    }

    /*update memory.*/
    token = strtok(NULL,"\t");
    if(!updateStringToMemory(token)){
      /*Error Saving data.*/
      return 0;
    }
    return 1;

  }

  if (!strcmp(token, EXTERN)) {
    if (hasSymbol) {
      symbol.type = INSTRUCTION;
      symbol.address.fullword.cell = 0;
      symbol.isExternal = 1;
      addSymbol(symbol);
      return 1;
    }

  }
  if (!strcmp(token, ENTRY)) {
    /* TODO: NO Idea what to do.*/
    return 1;
  }

  if(hasSymbol){
    symbol.type = ACTIONT;
    symbol.address.fullword.cell = (unsigned int)codewords.numberOfWords;
    symbol.isExternal = true;
    addSymbol(symbol);
  }

  wasFound = false;
  {
    int iaction = 0;
    while(iaction<ACTION_NUMBER){
      if(!strcmp(actions[iaction],token)){
        wasFound = true;
        actionID = iaction;
        printf("> %s Was found.\n", token);
        break;
      }
      iaction++;
    }
  }
  if (!wasFound) {
    /* code */
    printf("> ERROR (LINE %d) operator: '%s' Was not found! \n", lineNumber,token);
    return 0;
  }

  if (!checkSize(&codewords)) {
    /* ERROR ALLOCATING SPACE! */
    return 0;
  }
  word = createInstaceOfCommand((unsigned int)actionID);
  /*codewords.array[IC] = word;*/


/*FINISHED HERE! NEXT TIME FINISH FIRST WALKTHROUGH AND SECOND.*/
/*Check if parameter exist.*/

  {
    char* temp = strtok(NULL,"\n\t");
    if (temp==NULL&&word.command.grp!=NOP) {
      /*ERROR NO PARAMETERS*/
      printf("> ERROR NO PARAMETER!\n");
      token = NULL;
    }
    token = temp;
  }

  if (token == NULL&&word.command.grp!=NOP) {
    return 0;
  }
  if(!updateCommandParamToMemory(word,token)){
      /*ERROR: invalid parameter*/
    return 0;
  }






























return 1;

}
int addSymbol(Symbol symbol){
  int i = 0;

  if(!checkSymbolSize()){
    return 0;
  }


  while (i<symbols.numberOfSymbols) {
    /* code */
    if(!strcmp(symbols.array[i].name,symbol.name)){
    /*-------------------ERROR: Multiple declearation!----------------------*/
      printf("> ERROR: duplicate declearation: %s\n",symbol.name);
      return 0;
    }
    i++;
  }
  symbols.array[symbols.numberOfSymbols] = symbol;
  printf("> numberOfSymboles: %d\n",symbols.numberOfSymbols);
  printf("> Added: '%s' to Symbols\n",symbols.array[symbols.numberOfSymbols].name);
  symbols.numberOfSymbols++;
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
      if (!(isdigit(token[i]))&&(token[i]!='-')&&(token[i]!='+')) {
        /* code */
        printf("> ERROR: '%s' is invalid value!\n",token );
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
    if(!setWordValue(&value,(unsigned int)digitVal)){
      /*ERROR VALUE TOO BIG*/
      return 0;
    }
    datawords.array[datawords.numberOfWords] = value;
    printf("> The value %d was added to DATAWORDS on place:%d \n",datawords.array[datawords.numberOfWords].fullword.cell,datawords.numberOfWords );
    token = strtok(NULL,", \n");
    datawords.numberOfWords++;
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
      printf("> ERROR missing first -> Cytation <-\n");
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
      printf("> ERROR missing last -> Cytation <-\n");
      return 0;
    }
    while (i<lindex) {
      /* ~ERROR SAGMENT FAULT~ Fixed */
      Word val;
      if (!checkSize(&datawords)) {
        /* ERROR ALLOCATING SPACE! */

        return 0;
      }
      if(!setWordValue(&val,(unsigned int)token[i])){
        /*ERROR VALUE TOO BIG*/
        return 0;
      }
      datawords.array[datawords.numberOfWords] = val;
      printf("> The value %c (%d) was added to DATAWORDS on place:%d \n",datawords.array[datawords.numberOfWords].fullword.cell,datawords.array[datawords.numberOfWords].fullword.cell,datawords.numberOfWords );
      i++;
      datawords.numberOfWords++;
    }

    return 1;
}
int updateCommandParamToMemory(Word command,char* token){
  Word *word;/*remove!*/
  int error = 0;
  switch (command.command.grp) {
    case NOP:
    /*{
      int i = 0;

      while (i<strlen(token)) {
        if (token[i]==' '||token[i]=='\0'||token[i]=='\t') {
          i++;
        }
        error = 1;
        break;
      }
    }
    printf("DEBAG WORKS\n" );
    if (error) {
      printf("> Invalid Token, no parameter required: '%s'\n",token);
      return 0;
    }*/
    codewords.array[codewords.numberOfWords] = command;
    codewords.lines[codewords.numberOfWords] = lineNumber;
    word = &codewords.array[codewords.numberOfWords];
    codewords.numberOfWords++;

    return 1;
    case ONEOP:
    {
      int i = 0;
      while (i<strlen(token)) {
        if (token[i]==',') {
          /* ERROR TOO MANY PARAMS */
          printf("> ERROR: \"%s\" is an iligal parameter for this type of command.\n",token );
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
    codewords.array[codewords.numberOfWords] = command;
    codewords.lines[codewords.numberOfWords] = lineNumber;
    word = &codewords.array[codewords.numberOfWords];
    codewords.numberOfWords++;
    printf("> DEBAG 406: N: %d - S: %d\n", codewords.numberOfWords,codewords.size);
    if(!setUpCommandParams(&codewords.array[codewords.numberOfWords-1],token)){
      printf("> DEBAG ERROR!!!\n");
      return 0;
    }
    printf("> COMMAND GROUP:'%u' COMMAND OPCODE:'%u' COMMAND SRCAR:'%u' COMMAND DESTAR:'%u' \n",word->command.grp,word->command.opcode,word->command.srcar,word->command.destar);

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
    if (!checkSize(&codewords)) {
      /* ERROR ALLOCATING SPACE! */

      return 0;
    }
    codewords.array[codewords.numberOfWords] = command;
    codewords.lines[codewords.numberOfWords] = lineNumber;
    word = &codewords.array[codewords.numberOfWords];
    codewords.numberOfWords++;
    if(!setUpCommandParams(&codewords.array[codewords.numberOfWords-1],token)){
      printf("> DEBAG ERROR!!!\n");
      return 0;
    }
    printf("> COMMAND GROUP:'%u' COMMAND OPCODE:'%u' COMMAND SRCAR:'%u' COMMAND DESTAR:'%u' \n",word->command.grp,word->command.opcode,word->command.srcar,word->command.destar);









    return 1;
    default:
      break;
  }
  return 0;
}

int setUpCommandParams(Word* command,char *token){
  /*INSTANT_DYNAMIC_ADRESS_RESOLUTION structure...*/
  Word arg;
  int ERROR = 0;
  arg = createInstanceOfWord();
  printf("> PARAMS: '%s'\n",token);
  printf("> DEBAG 464: %d\n",command->command.opcode );
	switch (command->command.opcode) {
		case cmp:

    /*FIRST PARAM:   -------------ERROR STRTOK NOT WOKRING!!!------------*/
    if ((token = strtok(token," ,"))==NULL) {
      printf("> ERROR 469: '%s'\n",token );
      return 0;
    }
    printf("TOKEN %s\n",token );
    if(strlen(token)==0){
      printf("ERROR 474: Token has no arguments!\n");
      return 0;
    }
    if(isParamIAR(command,token,SOURCE)){
      ERROR = 0;
    }else if(isParamDRAR(command,token,SOURCE)){
      ERROR = 0;
    }else if (isParamIDAR(command,token,SOURCE)) {
      ERROR = 0;
    }else{
      if(!addUndefined(token)){
        return 0;
      }
      command->command.srcar = DIRECT_ADDRESS_RESOLUTION;
      codewords.array[codewords.numberOfWords] = arg;
      codewords.numberOfWords++;
    }
    /*--------------------------DESTINATION---------------------------*/
    if ((token = strtok(NULL," \t\n"))==NULL) {
      printf("> ERROR 490: '%s'\n",token );
      ERROR = 1;
      return !ERROR;
    }
    if(strlen(token)==0){
      printf("> ERROR 495: Token has no arguments!\n");
      ERROR = 1;
      return !ERROR;
    }
    if(isParamIAR(command,token,DESTINATION)){
      ERROR = 0;
    }else if(isParamDRAR(command,token,DESTINATION)){
      ERROR = 0;
    }else if (isParamIDAR(command,token,DESTINATION)) {
      ERROR = 0;
    }else{
        if(!addUndefined(token)){
            return 0;
        }
      command->command.srcar = DIRECT_ADDRESS_RESOLUTION;
      codewords.array[codewords.numberOfWords] = arg;
      codewords.numberOfWords++;
    }

		return !ERROR;
		case mov:
		case add:
        case sub:
    /*FIRST PARAM:   -------------ERROR STRTOK NOT WOKRING!!!------------*/
          /*TODO: Check validations! Such as:
           * "K[2-4] asd, LENGTH" - INVALID
           * "ab dv, askd" - INVALID
           * "asdas    , sdsd a" - INVALID
           * "asdasdas    ,      fsdfsdf " - VALID! <-----------
           * */
    if ((token = strtok(token," ,"))==NULL) {
      printf("> ERROR 517: '%s'\n",token );
      return 0;
    }
    if(strlen(token)==0){
      printf("ERROR 521: Token has no arguments!\n");
      return 0;
    }
    if(isParamIAR(command,token,SOURCE)){
      ERROR = 0;
    }else if(isParamDRAR(command,token,SOURCE)){
      ERROR = 0;
    }else if (isParamIDAR(command,token,SOURCE)) {
      ERROR = 0;
    }else{
        if(!addUndefined(token)){
            return 0;
        }
      command->command.srcar = DIRECT_ADDRESS_RESOLUTION;
      codewords.array[codewords.numberOfWords] = arg;
      codewords.numberOfWords++;
    }
    /*-----------------------DESTINATION------------------------------*/
    if ((token = strtok(NULL," \t\n"))==NULL) {
      printf("> ERROR 537: '%s'\n",token );
      ERROR = 1;
      return !ERROR;
    }
    if(strlen(token)==0){
      printf("> ERROR 542: Token has no arguments!\n");
      ERROR = 1;
      return !ERROR;
    }
    if(isParamIAR(command,token,DESTINATION)){
      ERROR = 1;
    }else if(isParamDRAR(command,token,DESTINATION)){
      ERROR = 0;
    }else if (isParamIDAR(command,token,DESTINATION)) {
      ERROR = 1;
    }else{
        if(!addUndefined(token)){
            return 0;
        }
      command->command.destar = DIRECT_ADDRESS_RESOLUTION;
      codewords.array[codewords.numberOfWords] = arg;
      codewords.numberOfWords++;
    }

    return !ERROR;

		case nt:
		case clr:
		case inc:
		case dec:
		case jmp:
		case bne:
		case red:
		case jsr:
    /*--------------------------DESTINATION---------------------------*/
    if ((token = strtok(token," \t\n"))==NULL) {
      printf("> ERROR 570: '%s'\n",token );
      ERROR = 1;
      return !ERROR;
    }
    if(strlen(token)==0){
      printf("> ERROR 575: Token has no arguments!\n");
      ERROR = 1;
      return !ERROR;
    }
    printf("> DEBAG 579: WORKING!\n");
    if(isParamIAR(command,token,DESTINATION)){
      ERROR = 1;
    }else if(isParamDRAR(command,token,DESTINATION)){
      ERROR = 0;
    }else if (isParamIDAR(command,token,DESTINATION)) {
      ERROR = 1;
    }else{
        if(!addUndefined(token)){
            return 0;
        }
      command->command.destar = DIRECT_ADDRESS_RESOLUTION;
      codewords.array[codewords.numberOfWords] = arg;
      codewords.numberOfWords++;
    }
    command->command.srcar = 0;


		return !ERROR;
		case prn:
    /*-----------------------DESTINATION------------------------------*/
    if ((token = strtok(token," \t\n"))==NULL) {
      printf("> ERROR 598: '%s'\n",token );
      ERROR = 1;
      return !ERROR;
    }
    if(strlen(token)==0){
      printf("> ERROR 603: Token has no arguments!\n");
      ERROR = 0;
      return !ERROR;
    }
    if(isParamIAR(command,token,DESTINATION)){
      ERROR = 0;
    }else if(isParamDRAR(command,token,DESTINATION)){
      ERROR = 0;
    }else if (isParamIDAR(command,token,DESTINATION)) {
      ERROR = 0;
    }else{
        if(!addUndefined(token)){
            return 0;
        }
      command->command.destar = DIRECT_ADDRESS_RESOLUTION;
      codewords.array[codewords.numberOfWords] = arg;
      codewords.numberOfWords++;
    }
    command->command.srcar = 0;
    return !ERROR;
		case lea:
    /*FIRST PARAM:   -------------------------*/
    if ((token = strtok(token," ,"))==NULL) {
      printf("> ERROR 623: '%s'\n",token );
      return 0;
    }
    if(strlen(token)==0){
      printf("ERROR 627: Token has no arguments!\n");
      return 0;
    }
    if(isParamIAR(command,token,SOURCE)){
      ERROR = 1;
    }else if(isParamDRAR(command,token,SOURCE)){
      ERROR = 1;
    }else if (isParamIDAR(command,token,SOURCE)) {
      ERROR = 1;
    }else{
        if(!addUndefined(token)){
            return 0;
        }
      command->command.srcar = DIRECT_ADDRESS_RESOLUTION;
      codewords.array[codewords.numberOfWords] = arg;
      codewords.numberOfWords++;
    }
    /*-----------------------DESTINATION------------------------------*/
    if ((token = strtok(NULL,"\t\n"))==NULL) {
      printf("> ERROR 643: '%s'\n",token );
      ERROR = 1;
      return !ERROR;
    }
    if(strlen(token)==0){
      printf("> ERROR 648: Token has no arguments!\n");
      ERROR = 1;
      return !ERROR;
    }
    if(isParamIAR(command,token,DESTINATION)){
      ERROR = 1;
    }else if(isParamDRAR(command,token,DESTINATION)){
      ERROR = 0;
    }else if (isParamIDAR(command,token,DESTINATION)) {
      ERROR = 1;
    }else{
        if(!addUndefined(token)){
            return 0;
        }
      command->command.destar = DIRECT_ADDRESS_RESOLUTION;
      codewords.array[codewords.numberOfWords] = arg;
      codewords.numberOfWords++;
    }
    return !ERROR;
    default:
    printf("> ERROR LINE 476  \n");
    return 1;

	}
}
int isParamIAR(Word *command,char* token,int location){
  Word arg = createInstanceOfWord();
  if(token[0] == '#'){
    if(location == SOURCE){
      command->command.srcar = INSTANT_ADDRESS_RESOLUTION;
    }
    if(location == DESTINATION){
      command->command.destar = INSTANT_ADDRESS_RESOLUTION;
    }
    arg.pvalue.value = (int)strtol(token+1,NULL, 10);
    codewords.array[codewords.numberOfWords] = arg;
    codewords.numberOfWords++;
    printf("Argument: '%d' CommandId: '%u' Command source address resolution: '%u' \n",arg.pvalue.value,codewords.array[codewords.numberOfWords-2].command.opcode,codewords.array[codewords.numberOfWords-2].command.srcar );
    return 1;
  }
  /*Raw number was not found.*/
  return 0;
}
int isParamDRAR(Word *command,char* token,int location){
  Word arg = createInstanceOfWord();
  int i = 0;

  while (i < REGISTERS_NUMER) {
    if (!strcmp(registers[i],token)) {
      if(location == SOURCE){
        command->command.srcar = DIRECT_REGISTER_ADDRESS_RESOLUTION;
        arg.paddress.src = i;
        codewords.array[codewords.numberOfWords] = arg;
        codewords.numberOfWords++;
      }
      if(location == DESTINATION){
        if ((command->command.grp == TWOP)&&(command->command.srcar == DIRECT_REGISTER_ADDRESS_RESOLUTION)) {
          codewords.array[codewords.numberOfWords-1].paddress.dest = i;
        }else{
          arg.paddress.dest = i;
          codewords.array[codewords.numberOfWords] = arg;
          codewords.numberOfWords++;
        }
        command->command.destar = DIRECT_REGISTER_ADDRESS_RESOLUTION;
      }

      return 1;
    }
    i++;
  }
  /*Register was not found.*/
  return 0;

}
int isParamIDAR(Word *command,char* token,int location){
  Word arg = createInstanceOfWord();
  int structureIndex = 0;
  int const NUMBEROFSTRUCTURE = 3;
  char idarStructure[] = {'[','-',']'};
  int i = 0;
  int symbEnd = 0;
  while (i<strlen(token)) {
    if (structureIndex>=NUMBEROFSTRUCTURE) {
      break;
    }
    if (token[i]==idarStructure[structureIndex]) {
      /* code */
      if(structureIndex == 0){
        symbEnd = i;
      }
      structureIndex++;
    }
    i++;
  }
  if(symbEnd>0){
    Undefined und;
    und.size = 0;
    und.numberOfShows = 0;
    und.shows = NULL;
    und.name = NULL;
    checkUndSize(&und);
    memcpy(und.name,token,symbEnd);
    und.name[symbEnd] = '\0';
    und.shows[und.numberOfShows] = lineNumber;
    und.numberOfShows++;
    unds.array[unds.numberOfUnd] = und;
    unds.numberOfUnd++;
    printf("> DEBAG 764: %s \n",und.name);
  }
  if (structureIndex==NUMBEROFSTRUCTURE) {
    if(location == SOURCE){
      command->command.srcar = INSTANT_DYNAMIC_ADDRESS_RESOLUTION;
    }
    if(location == DESTINATION){
      command->command.destar = INSTANT_DYNAMIC_ADDRESS_RESOLUTION;
    }
    codewords.array[codewords.numberOfWords] = arg;
    codewords.numberOfWords++;
    return 1;
  }
  /*Structure was not found.*/
  return 0;
}
void printArr() {
  printInstructionsArray(&codewords);
}
int checkSymbolSize(){
  if (symbols.size <= symbols.numberOfSymbols+2) {
    /* code */
    symbols.size += 10;
    if((symbols.array = realloc(symbols.array,sizeof(Symbol)*symbols.size))==NULL){
      printf("> ERROR! not enogh space\n");
      return 0;
    }

  }
  return 1;
}

/*check Undefined item*/
int checkUndSize(Undefined *und){
  if (!(und->name)) {
      if(!(und->name = (char*)malloc(kMAX_SYMBOL_NAME_SIZE*sizeof(char)))){
          return 0;
      }
  }
	if (und->size<=(und->numberOfShows+3)) {

    und->size += 3;
    /*WARNING: DONT REALLOC DIRECTLY TO SAME PARAM!-----------------------------------------*/
    if ((und->shows = (int*)realloc(und->shows, (und->size)*sizeof(int)))==NULL) {
      /* code */
      printf(">    ERROR NOT ENOGH SPACE!!!\n");
			return 0;
    }
    printf(">   ARRAY RESIZED!\n");
		return 1;
  }
	return 1;

}
/*Check the Undefined list*/
int checkUndsSize(Undefineds *und){
	if (und->size<=(und->numberOfUnd+3)) {

    und->size += 3;
    /*WARNING: DONT REALLOC DIRECTLY TO SAME PARAM!-----------------------------------------*/
    if ((und->array = (Undefined*)realloc(und->array, sizeof(Undefined)*und->size))==NULL) {
      /* code */
      printf(">    ERROR NOT ENOGH SPACE!!!\n");
			return 0;
    }
    printf(">   ARRAY RESIZED!\n");
		return 1;
  }
	return 1;

}
int addUndefined(char *token){
  int i = 0;
  Undefined und;
  while (i<unds.numberOfUnd) {
      /*Trying to fined more of 
       */
    if(!strcmp(token,unds.array[i].name)){
      checkUndSize(&unds.array[i]);
      unds.array[i].shows[unds.array[i].numberOfShows] = lineNumber;
      unds.array[i].numberOfShows++;
      return 1;
    }
      i++;
  }
  und.size = 0;
  und.numberOfShows = 0;
  und.shows = NULL;
  und.name = NULL;
  checkUndSize(&und);
  strcpy(und.name,token);
    und.shows[und.numberOfShows] = lineNumber;
    und.numberOfShows++;
  unds.array[unds.numberOfUnd] = und;
  unds.numberOfUnd++;
  return 1;
}

void printUndefineds(){
    int i;
    printf("----------------UNDIFINEDS--------------\n");
    for (i=0; unds.numberOfUnd>i; i++) {
        int j;
        printf("The Undefined: '%s' has %d shows in lines: ",unds.array[i].name, unds.array[i].numberOfShows);
        for (j = 0; j<unds.array[i].numberOfShows; j++) {
            printf("%d, ",unds.array[i].shows[j]);
        }
        printf("\n");
    }
    printf("--------------------------------------\n");
}
