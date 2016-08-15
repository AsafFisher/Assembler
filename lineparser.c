
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "lineparser.h"
#include "protocols.h"
#include "assembler.h"
#include "legality.h"
char *actions[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr",
                   "rts", "stop"};
char *registers[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7‬‬"};
#define ACTION_NUMBER 16
#define REGISTERS_NUMER 8
#define SOURCE 0
#define DESTINATION 1

#define MEMORY_START 100
#define MAX_MEMORY 1000
#define kMAX_SYMBOL_NAME_SIZE 20
#define DATA ".data"
#define STRING ".string"
#define EXTERN ".extern"
#define ENTRY ".entry"

enum {
    false = 0, true
};
Words codewords;
Words datawords;
Symbols symbols;
int lineNumber;

int addSymbol(Symbol symbol);

int setSymbolValue(char *line, Word *location);

int updateDataToMemory(char *token);

int updateStringToMemory(char *token);

int updateCommandParamToMemory(Word command, char *token);

int setUpCommandParams(Word *word, char *token);

int base10ToBase8(int number);

int identifyIDARSymbol(char* idarStr,Word *location);


char base8Symbol(int number);

int freeAll();

Symbol* getSymbolByName(char* param);

Word cropRangeFromValue(Word value, int startBit, int endBit);
/*Check if param is INSTANT_ADDRESS_RESOLUTION, create a Word and save space in memory.
* location is the type of the assignment (SOURCE OR DESTINATION).
*/
int isParamIAR(Word *command, char *token, int location);

/*Check if param is DIRECT_REGISTER_ADDRESS_RESOLUTION, create a Word and save space in memory.*/
int isParamDRAR(Word *command, char *token, int location);

/*Check if param is INSTANT_DYNAMIC_ADDRESS_RESOLUTION, create a Word and save space in memory.*/
int isParamIDAR(Word *command, char *token, int location);

int checkSymbolSize();








int parseLine(char *buff, int number) {
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
	
	if(!is_legal_line( buff,lineNumber))
		return 0;
    if (!(token = strtok(buff, " \n"))) {
        return 1;
    }
    if (token[strlen(token) - 1] == ':') {
        /* code */

        /*Remove the : from the Symbole*/
        hasSymbol = true;
        token[strlen(token) - 1] = '\0';
        symbol.name = malloc(strlen(token) + 1);
        strcpy(symbol.name, token);
        token = strtok(NULL, " \n");
    }

    /*-------------------Detect DATA numbers and store them + detect data Symbols-------------------------*/
    if (!strcmp(token, DATA)) {
        /* code */
        if (hasSymbol) {
            /* Symbole exist: */
            symbol.type = INSTRUCTION;
            symbol.address.pvalue.value = datawords.numberOfWords;
            symbol.isExternal = false;
            if (!addSymbol(symbol)) {
                /*ERROR!*/
                return 0;
            }
        }
        token = strtok(NULL, "\t");

        if (!updateDataToMemory(token)) {
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
            symbol.address.pvalue.value = datawords.numberOfWords;
            symbol.isExternal = false;
            if (!addSymbol(symbol)) {
                /*ERROR!*/
                return 0;
            }
        }

        /*update memory.*/
        token = strtok(NULL, "\t");
        if (!updateStringToMemory(token)) {
            /*Error Saving data.*/
            return 0;
        }
        return 1;

    }

    if (!strcmp(token, EXTERN)) {
        if (hasSymbol) {
            printf("> Warning: label not needed. LINE: %d",lineNumber);
        }
        /*Add external symbol.*/
        if ((token = strtok(NULL, "\n")) != NULL) {
            symbol.name = malloc(strlen(token) + 1);
            strcpy(symbol.name, token);
            symbol.type = INSTRUCTION;
            symbol.address.pvalue.value = 0;
            symbol.isExternal = true;
            /*TODO: CHECK IF SYMBOL RETURN 0 IF SYMBOL NOT FOUND.*/
            if(!addSymbol(symbol)){
                return 0;
            }
            return 1;
        }


    }

    if (!strcmp(token, ENTRY)) {
        /* TODO: NO Idea what we are doing, but QA said we need to finish it.*/
        return 1;
    }

    if (hasSymbol) {
        symbol.type = ACTIONT;
        symbol.address.pvalue.value = codewords.numberOfWords;
        symbol.isExternal = false;
        if(!addSymbol(symbol)){
            return 0;
        }
    }

    wasFound = false;
    {
        int iaction = 0;
        while (iaction < ACTION_NUMBER) {
            if (!strcmp(actions[iaction], token)) {
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
        printf("> ERROR (LINE %d) operator: '%s' Was not found! \n", lineNumber, token);
        return 0;
    }

    if (!checkSize(&codewords)) {
        /* ERROR ALLOCATING SPACE! */
        return 0;
    }
    word = createInstaceOfCommand((unsigned int) actionID);
    /*codewords.array[IC] = word;*/


/*FINISHED HERE! NEXT TIME FINISH FIRST WALKTHROUGH AND SECOND.*/
/*Check if parameter exist.*/

    {
        char *temp = strtok(NULL, "\n\t");
        if (temp == NULL && word.command.grp != NOP) {
            /*ERROR NO PARAMETERS*/
            printf("> ERROR NO PARAMETER!\n");
            token = NULL;
        }
        token = temp;
    }

    if (token == NULL && word.command.grp != NOP) {
        return 0;
    }
    if (!updateCommandParamToMemory(word, token)) {
        /*ERROR: invalid parameter*/
        return 0;
    }


    return 1;

}
int variableLinker(FILE *input) {
    int commandIndex = 0;
    int currentFileLine = 0;
    while (commandIndex < codewords.numberOfWords) {
        Word *currentWord = codewords.array + commandIndex;
        int currentCommandLine = codewords.lines[commandIndex];
        char line[LINE_MAX];
        char *temp = NULL;
        while(currentFileLine<currentCommandLine){
            if(fgets(line,LINE_MAX,input)==NULL){
                return 0;
            }
            currentFileLine++;
        }


        if(strstr(line,":")!=NULL)
            temp = strtok(line, ":");
        if(temp == NULL){
            temp = strtok(line, "\n");
        }else{
            temp = strtok(NULL, "\n");
        }

        if (currentWord->command.grp == NOP) {
            /*No operators next command is right at commandIndex+1*/
            commandIndex++;
        }
        if (currentWord->command.grp == ONEOP) {
            /*One operator, variable is located at commandIndex+1 next command is right at commandIndex+2*/
            int varIndex = commandIndex + 1;
            strtok(temp, " ");
            temp = strtok(NULL," \n");


            switch(currentWord->command.destar){
                case DIRECT_ADDRESS_RESOLUTION:
                    /*Search in the */
                    if(!setSymbolValue(temp,&codewords.array[varIndex])){
                        printf("> To check error location please go to line: %d.\n",currentCommandLine);
                        return 0;
                    }
                    break;
                    /*TODO: FINISH Second case and Two operators*/
                case INSTANT_DYNAMIC_ADDRESS_RESOLUTION:

                    if(!identifyIDARSymbol(temp,&codewords.array[varIndex])){
                        printf("> To check error location please go to line: %d.\n",currentCommandLine);
                        return 0;
                    }



                    break;
                default:
                    break;
            }





            commandIndex += 2;
            continue;
        }
        if (currentWord->command.grp == TWOP) {
            int firstVarIndex, secondVarIndex;
            if (currentWord->command.destar == DIRECT_REGISTER_ADDRESS_RESOLUTION &&
                currentWord->command.srcar == DIRECT_REGISTER_ADDRESS_RESOLUTION) {
                /*No need to do anything because Registers has already been taken care of.*/
                commandIndex+=2;
                continue;
            }
            /*Two operators, variables located at commandIndex+1 second variable located at commandIndex+2 next command is at commandIndex+3*/
            firstVarIndex = commandIndex + 1;
            secondVarIndex = commandIndex + 2;
            /*One operator next command is right at commandIndex+2*/
            strtok(temp, " ");
            temp = strtok(NULL," ,");
            /*Source*/
            switch(currentWord->command.srcar){
                case DIRECT_ADDRESS_RESOLUTION:
                    if(!setSymbolValue(temp,&codewords.array[firstVarIndex])){
                        printf("> To check error location please go to line: %d.\n",currentCommandLine);
                        return 0;
                    }
                    break;
                case INSTANT_DYNAMIC_ADDRESS_RESOLUTION:
                    if(!identifyIDARSymbol(temp,&codewords.array[firstVarIndex])){
                        printf("> To check error location please go to line: %d.\n",currentCommandLine);
                        return 0;
                    }
                    break;
                default:
                    break;
            }
            temp = strtok(NULL," ,\n");
            /*TODO: Finish debagging here.*/
            /*Destenation*/
            switch(currentWord->command.destar){
                case DIRECT_ADDRESS_RESOLUTION:
                    if(!setSymbolValue(temp,&codewords.array[secondVarIndex])){
                        printf("> To check error location please go to line: %d.\n",currentCommandLine);
                        return 0;
                    }
                    break;
                case INSTANT_DYNAMIC_ADDRESS_RESOLUTION:
                    if(!identifyIDARSymbol(temp,&codewords.array[secondVarIndex])){
                        printf("> To check error location please go to line: %d.\n",currentCommandLine);
                        return 0;
                    }
                    break;
                default:
                    break;
            }

            commandIndex += 3;
            continue;
        }

    }
    return 1;

}

/*Code Words has a special built at */
/*Gets a parameter and a Word and replace the contained in the word with the Address of the symbol (return 0 if symbol does not exist)*/
int setSymbolValue(char *param, Word *location){
    int i = 0;
    Symbol* symbol;
    if((symbol = getSymbolByName(param))==NULL){
        printf("> ERROR: variable named '%s' was never declared!\n",param);
        return 0;
    }
    if(symbol->isExternal){
        location->fullword.cell = symbol->address.fullword.cell;
        return 1;
    }
    location->pvalue.value = symbol->address.pvalue.value+MEMORY_START+codewords.numberOfWords;
    return 1;
}
/*return pointer of symbol or NULL if not found.*/
Symbol* getSymbolByName(char* param){
    int i = 0;
    while(i<symbols.numberOfSymbols){
        if(!strcmp(param,symbols.array[i].name)){
            /*Found symbol*/
            return &symbols.array[i];
        }

        i++;
    }
    return NULL;
}

int addSymbol(Symbol symbol) {
    int i = 0;

    if (!checkSymbolSize()) {
        return 0;
    }


    while (i < symbols.numberOfSymbols) {
        /* code */
        if (!strcmp(symbols.array[i].name, symbol.name)) {
            /*-------------------ERROR: Multiple declearation!----------------------*/
            printf("> ERROR: duplicate declearation: %s\n", symbol.name);
            return 0;
        }
        i++;
    }
    symbols.array[symbols.numberOfSymbols] = symbol;
    printf("> numberOfSymboles: %d\n", symbols.numberOfSymbols);
    printf("> Added: '%s' to Symbols\n", symbols.array[symbols.numberOfSymbols].name);
    symbols.numberOfSymbols++;
    return 1;
}

int updateDataToMemory(char *token) {
    Word value;
    token = strtok(token, ", \n");
    while (token != NULL) {
        long digitVal;
        int i = 0;

        /*TODO:
        1. If Chars in mid mark error.
        2. Convert string to number.
        */

        while (i < strlen(token)) {
            /* code */
            if (!(isdigit(token[i])) && (token[i] != '-') && (token[i] != '+')) {
                /* code */
                printf("> ERROR: '%s' is invalid value!\n", token);
                return 0;
            }
            i++;
        }
        /*Get int value of the token!*/
        digitVal = strtol(token, NULL, 10);
        if (!checkSize(&datawords)) {
            /* ERROR ALLOCATING SPACE! */

            return 0;
        }
        printf("DATA value: %d\n", (int) digitVal);
        if (!setWordValue(&value, (unsigned int) digitVal)) {
            /*ERROR VALUE TOO BIG*/
            return 0;
        }
        datawords.array[datawords.numberOfWords] = value;
        printf("> The value %d was added to DATAWORDS on place:%d \n",
               datawords.array[datawords.numberOfWords].fullword.cell, datawords.numberOfWords);
        token = strtok(NULL, ", \n");
        datawords.numberOfWords++;
    }
    return 1;
}

int updateStringToMemory(char *token) {
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
    while (i < (strlen(token) - 1)) {
        if (token[i] == '\"') {
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
    while (lindex < (strlen(token) - 1)) {
        if (token[lindex] == '\"') {
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
    while (i < lindex) {
        /* ~ERROR SAGMENT FAULT~ Fixed */
        Word val;
        if (!checkSize(&datawords)) {
            /* ERROR ALLOCATING SPACE! */

            return 0;
        }
        if (!setWordValue(&val, (unsigned int) token[i])) {
            /*ERROR VALUE TOO BIG*/
            return 0;
        }
        datawords.array[datawords.numberOfWords] = val;
        i++;
        datawords.numberOfWords++;
    }
    Word val;
    setWordValue(&val,0);
    datawords.array[datawords.numberOfWords] = val;
    datawords.numberOfWords++;


    return 1;
}

int updateCommandParamToMemory(Word command, char *token) {
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
        case ONEOP: {
            int i = 0;
            while (i < strlen(token)) {
                if (token[i] == ',') {
                    /* ERROR TOO MANY PARAMS */
                    printf("> ERROR: \"%s\" is an iligal parameter for this type of command.\n", token);
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
            printf("> DEBAG 406: N: %d - S: %d\n", codewords.numberOfWords, codewords.size);
            if (!setUpCommandParams(&codewords.array[codewords.numberOfWords - 1], token)) {
                printf("> DEBAG ERROR!!!\n");
                return 0;
            }
            printf("> COMMAND GROUP:'%u' COMMAND OPCODE:'%u' COMMAND SRCAR:'%u' COMMAND DESTAR:'%u' \n",
                   word->command.grp, word->command.opcode, word->command.srcar, word->command.destar);

            return 1;
        case TWOP: {
            int found = 0;
            int i = 0;
            while (i < strlen(token)) {
                if (token[i] == ',') {
                    found = 1;
                    break;
                }
                i++;
            }
            if (!found) {
                printf("Missing parameter: %s\n", token);
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
            if (!setUpCommandParams(&codewords.array[codewords.numberOfWords - 1], token)) {
                printf("> DEBAG ERROR!!!\n");
                return 0;
            }
            printf("> COMMAND GROUP:'%u' COMMAND OPCODE:'%u' COMMAND SRCAR:'%u' COMMAND DESTAR:'%u' \n",
                   word->command.grp, word->command.opcode, word->command.srcar, word->command.destar);


            return 1;
        default:
            break;
    }
    return 0;
}

int setUpCommandParams(Word *command, char *token) {
    /*INSTANT_DYNAMIC_ADDRESS_RESOLUTION structure...*/
    Word arg;
    int ERROR = 0;
    arg = createInstanceOfWord();
    switch (command->command.opcode) {
        case cmp:

            if ((token = strtok(token, " ,")) == NULL) {
                printf("> ERROR 469: '%s'\n", token);
                return 0;
            }
            printf("TOKEN %s\n", token);
            if (strlen(token) == 0) {
                printf("ERROR 474: Token has no arguments!\n");
                return 0;
            }
            if (isParamIAR(command, token, SOURCE)) {
                ERROR = 0;
            } else if (isParamDRAR(command, token, SOURCE)) {
                ERROR = 0;
            } else if (isParamIDAR(command, token, SOURCE)) {
                ERROR = 0;
            } else {
                command->command.srcar = DIRECT_ADDRESS_RESOLUTION;
                arg.paddress.ERA = RELOCATABLE;
                codewords.array[codewords.numberOfWords] = arg;
                codewords.numberOfWords++;
            }
            /*--------------------------DESTINATION---------------------------*/
            if ((token = strtok(NULL, " \t\n")) == NULL) {
                printf("> ERROR 490: '%s'\n", token);
                ERROR = 1;
                return !ERROR;
            }
            if (strlen(token) == 0) {
                printf("> ERROR: Token has no arguments! LINE: %d\n",lineNumber);
                ERROR = 1;
                return !ERROR;
            }
            if (isParamIAR(command, token, DESTINATION)) {
                ERROR = 0;
            } else if (isParamDRAR(command, token, DESTINATION)) {
                ERROR = 0;
            } else if (isParamIDAR(command, token, DESTINATION)) {
                ERROR = 0;
            } else {
                command->command.srcar = DIRECT_ADDRESS_RESOLUTION;
                arg.paddress.ERA = RELOCATABLE;
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
            if ((token = strtok(token, " ,")) == NULL) {
                printf("> ERROR 517: '%s'\n", token);
                return 0;
            }
            if (strlen(token) == 0) {
                printf("ERROR 521: Token has no arguments!\n");
                return 0;
            }
            if (isParamIAR(command, token, SOURCE)) {
                ERROR = 0;
            } else if (isParamDRAR(command, token, SOURCE)) {
                ERROR = 0;
            } else if (isParamIDAR(command, token, SOURCE)) {
                ERROR = 0;
            } else {
                command->command.srcar = DIRECT_ADDRESS_RESOLUTION;
                arg.paddress.ERA = RELOCATABLE;
                codewords.array[codewords.numberOfWords] = arg;
                codewords.numberOfWords++;
            }
            /*-----------------------DESTINATION------------------------------*/
            if ((token = strtok(NULL, " \t\n")) == NULL) {
                printf("> ERROR 537: '%s'LINE: %d\n", token,lineNumber);
                ERROR = 1;
                return !ERROR;
            }
            if (strlen(token) == 0) {
                printf("> ERROR: Token has no arguments! LINE: %d\n",lineNumber);
                ERROR = 1;
                return !ERROR;
            }
            if (isParamIAR(command, token, DESTINATION)) {
                printf("Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
                ERROR = 1;
            } else if (isParamDRAR(command, token, DESTINATION)) {
                ERROR = 0;
            } else if (isParamIDAR(command, token, DESTINATION)) {
                printf("Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
                ERROR = 1;
            } else {
                command->command.destar = DIRECT_ADDRESS_RESOLUTION;
                arg.paddress.ERA = RELOCATABLE;
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
            if ((token = strtok(token, " \t\n")) == NULL) {
                printf("> ERROR 570: '%s'\n", token);
                ERROR = 1;
                return !ERROR;
            }
            if (strlen(token) == 0) {
                printf("> ERROR 575: Token has no arguments!\n");
                ERROR = 1;
                return !ERROR;
            }
            printf("> DEBAG 579: WORKING!\n");
            if (isParamIAR(command, token, DESTINATION)) {
                printf("Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
                ERROR = 1;
            } else if (isParamDRAR(command, token, DESTINATION)) {
                ERROR = 0;
            } else if (isParamIDAR(command, token, DESTINATION)) {
                printf("Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
                ERROR = 1;
            } else {
                command->command.destar = DIRECT_ADDRESS_RESOLUTION;
                arg.paddress.ERA = RELOCATABLE;
                codewords.array[codewords.numberOfWords] = arg;
                codewords.numberOfWords++;
            }
            command->command.srcar = 0;


            return !ERROR;
        case prn:
            /*-----------------------DESTINATION------------------------------*/
            if ((token = strtok(token, " \t\n")) == NULL) {
                printf("> ERROR 598: '%s'\n", token);
                ERROR = 1;
                return !ERROR;
            }
            if (strlen(token) == 0) {
                printf("> ERROR 603: Token has no arguments!\n");
                ERROR = 0;
                return !ERROR;
            }
            if (isParamIAR(command, token, DESTINATION)) {
                ERROR = 0;
            } else if (isParamDRAR(command, token, DESTINATION)) {
                ERROR = 0;
            } else if (isParamIDAR(command, token, DESTINATION)) {
                ERROR = 0;
            } else {
                command->command.destar = DIRECT_ADDRESS_RESOLUTION;
                arg.paddress.ERA = RELOCATABLE;
                codewords.array[codewords.numberOfWords] = arg;
                codewords.numberOfWords++;
            }
            command->command.srcar = 0;
            return !ERROR;
        case lea:
            /*FIRST PARAM:   -------------------------*/
            if ((token = strtok(token, " ,")) == NULL) {
                printf("> ERROR 623: '%s'\n", token);
                return 0;
            }
            if (strlen(token) == 0) {
                printf("ERROR 627: Token has no arguments!\n");
                return 0;
            }
            if (isParamIAR(command, token, SOURCE)) {
                printf("Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
                ERROR = 1;
            } else if (isParamDRAR(command, token, SOURCE)) {
                printf("Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
                ERROR = 1;
            } else if (isParamIDAR(command, token, SOURCE)) {
                printf("Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
                ERROR = 1;
            } else {
                command->command.srcar = DIRECT_ADDRESS_RESOLUTION;
                arg.paddress.ERA = RELOCATABLE;
                codewords.array[codewords.numberOfWords] = arg;
                codewords.numberOfWords++;
            }
            /*-----------------------DESTINATION------------------------------*/
            if ((token = strtok(NULL, "\t\n")) == NULL) {
                printf("> ERROR 643: '%s'\n", token);
                ERROR = 1;
                return !ERROR;
            }
            if (strlen(token) == 0) {
                printf("> ERROR 648: Token has no arguments!\n");
                ERROR = 1;
                return !ERROR;
            }
            if (isParamIAR(command, token, DESTINATION)) {
                printf("Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
                ERROR = 1;
            } else if (isParamDRAR(command, token, DESTINATION)) {
                ERROR = 0;
            } else if (isParamIDAR(command, token, DESTINATION)) {
                printf("Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
                ERROR = 1;
            } else {
                command->command.destar = DIRECT_ADDRESS_RESOLUTION;
                arg.paddress.ERA = RELOCATABLE;
                codewords.array[codewords.numberOfWords] = arg;
                codewords.numberOfWords++;
            }
            return !ERROR;
        default:
            printf("> ERROR Wrong Abort.\n");
            return 0;

    }
}

int isParamIAR(Word *command, char *token, int location) {
    Word arg = createInstanceOfWord();
    if (token[0] == '#') {
        if (location == SOURCE) {
            command->command.srcar = INSTANT_ADDRESS_RESOLUTION;
        }
        if (location == DESTINATION) {
            command->command.destar = INSTANT_ADDRESS_RESOLUTION;
        }
        arg.pvalue.value = (int) strtol(token + 1, NULL, 10);
        codewords.array[codewords.numberOfWords] = arg;
        codewords.numberOfWords++;
        return 1;
    }
    /*Raw number was not found.*/
    return 0;
}

int isParamDRAR(Word *command, char *token, int location) {
    Word arg = createInstanceOfWord();
    arg.paddress.ERA = 0;
    int i = 0;

    while (i < REGISTERS_NUMER) {
        if (!strcmp(registers[i], token)) {
            if (location == SOURCE) {
                command->command.srcar = DIRECT_REGISTER_ADDRESS_RESOLUTION;
                arg.paddress.src = i;
                codewords.array[codewords.numberOfWords] = arg;
                codewords.numberOfWords++;
            }
            if (location == DESTINATION) {
                if ((command->command.grp == TWOP) && (command->command.srcar == DIRECT_REGISTER_ADDRESS_RESOLUTION)) {
                    codewords.array[codewords.numberOfWords - 1].paddress.dest = i;
                } else {
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

int isParamIDAR(Word *command, char *token, int location) {
    Word arg = createInstanceOfWord();
    int structureIndex = 0;
    int const NUMBEROFSTRUCTURE = 3;
    char idarStructure[] = {'[', '-', ']'};
    int i = 0;
    int symbEnd = 0;
    while (i < strlen(token)) {
        if (structureIndex >= NUMBEROFSTRUCTURE) {
            break;
        }
        if (token[i] == idarStructure[structureIndex]) {
            /* code */
            if (structureIndex == 0) {
                symbEnd = i;
            }
            structureIndex++;
        }
        i++;
    }

    if (structureIndex == NUMBEROFSTRUCTURE) {
        if (location == SOURCE) {
            command->command.srcar = INSTANT_DYNAMIC_ADDRESS_RESOLUTION;
        }
        if (location == DESTINATION) {
            command->command.destar = INSTANT_DYNAMIC_ADDRESS_RESOLUTION;
        }
        codewords.array[codewords.numberOfWords] = arg;
        codewords.numberOfWords++;
        return 1;
    }
    /*Structure was not found.*/
    return 0;
}

int identifyIDARSymbol(char* idarStr, Word* location){
    int i = 0;
    int endOfName;
    int from = 0;
    int to = 0;
    int foundMid = 0;
    Symbol* symbol;
    Word value;
    char symbolName[kMAX_SYMBOL_NAME_SIZE];
    while(idarStr[i]!=NULL){
        if(idarStr[i]=='['){
            endOfName = i;
            i++;
            continue;
        }
        if(idarStr[i]=='-'){
            foundMid = i;
            i++;
            continue;
        }
        if(foundMid == 0) {
            if (endOfName != 0) {
                from = atoi(idarStr + i);
            }
        }else{
            if (endOfName != 0) {
                to = atoi(idarStr+i);
                break;
            }

        }
        /*DO ATOI*/

        i++;
    }
    memcpy(symbolName,idarStr,endOfName);
    symbol = getSymbolByName(symbolName);
    if(symbol == NULL){
        printf("> ERROR: variable named '%s' was never declared!\n",symbolName);
        return 0;
    }
    switch (symbol->type){
        case INSTRUCTION:
            if(from<0&&from>=13){
                printf("> ERROR (LINE: %d): The value '%d' must be between 0 to 12 ",lineNumber,from);
                return 0;
            }
            if(to<0&&to>=13){
                printf("> ERROR (LINE: %d): The value '%d' must be between 0 to 12 ",lineNumber,to);
                return 0;
            }
            value = cropRangeFromValue(datawords.array[symbol->address.pvalue.value], from, to);
            location->fullword.cell = value.fullword.cell;
            return 1;
        case ACTIONT:
            if(from<0&&from>=13){
                printf("> ERROR (LINE: %d): The value '%d' must be between 0 to 12 ",lineNumber,from);
                return 0;
            }
            if(to<0&&to>=13){
                printf("> ERROR (LINE: %d): The value '%d' must be between 0 to 12 ",lineNumber,to);
                return 0;
            }
            value = cropRangeFromValue(codewords.array[symbol->address.pvalue.value], from, to);
            location->fullword.cell = value.fullword.cell;
    }



    return 1;
}

int checkSymbolSize() {
    if (symbols.size <= symbols.numberOfSymbols + 2) {
        /* code */
        symbols.size += 10;
        if ((symbols.array = realloc(symbols.array, sizeof(Symbol) * symbols.size)) == NULL) {
            printf("> ERROR! not enogh space\n");
            return 0;
        }

    }
    return 1;
}

Word cropRangeFromValue(Word value, int startBit, int endBit){
    /*TODO: Negative number.*/
    Word newValue = createInstanceOfWord();
    int mask = ~0;
    newValue.fullword.cell = value.fullword.cell;
    newValue.fullword.cell>>=startBit;
    mask<<=(endBit-startBit+1);
    mask = ~mask;
    newValue.fullword.cell&=mask;
    mask = 1;
    mask<<=(endBit-startBit);
    if(newValue.fullword.cell&mask){
        mask = ~0;
        mask<<=(endBit-startBit);
        newValue.fullword.cell|=mask;
    }
    newValue.fullword.cell<<=2;


    return newValue;
}

void printSymbols(void) {
    int i;
    printf("----------------Symbols--------------\n");
    for (i = 0; symbols.numberOfSymbols > i; i++) {
        int j;
        printf("The Symbol: '%s' has an address of: '%u' isExternal = '%d' Type: '%d' ", symbols.array[i].name,
               symbols.array[i].address.fullword.cell, symbols.array[i].isExternal, symbols.array[i].type);
        printf("\n");
    }
    printf("--------------------------------------\n");
}

void printArr() {
    printInstructionsArray(&codewords);
}
char* convertToBase8(){
    char* translated = calloc(LINE_MAX*(codewords.numberOfWords+datawords.numberOfWords),sizeof(char));
    int arrayIndex = 0;
    int i = 0;
    char base8[MAX_MEMORY];
    unsigned int address = 100;
    Word datalength = createInstanceOfWord();
    Word codelength = createInstanceOfWord();
    datalength.fullword.cell = datawords.numberOfWords;
    codelength.fullword.cell = codewords.numberOfWords;

    translated[arrayIndex] = base8Symbol(codelength.base8.block4);
    arrayIndex++;
    translated[arrayIndex] = base8Symbol(codelength.base8.block3);
    arrayIndex++;
    translated[arrayIndex] = base8Symbol(codelength.base8.block2);
    arrayIndex++;
    translated[arrayIndex] = base8Symbol(codelength.base8.block1);
    arrayIndex++;

    translated[arrayIndex] = ' ';
    arrayIndex++;

    translated[arrayIndex] = base8Symbol(datalength.base8.block4);
    arrayIndex++;
    translated[arrayIndex] = base8Symbol(datalength.base8.block3);
    arrayIndex++;
    translated[arrayIndex] = base8Symbol(datalength.base8.block2);
    arrayIndex++;
    translated[arrayIndex] = base8Symbol(datalength.base8.block1);
    arrayIndex++;

    translated[arrayIndex] = '\n';
    arrayIndex++;

    i = 0;
    while(i<codewords.numberOfWords){
        Word temp = createInstanceOfWord();
        /*Address format*/
        temp.fullword.cell = address;
        translated[arrayIndex] = base8Symbol(temp.base8.block4);
        arrayIndex++;
        translated[arrayIndex] = base8Symbol(temp.base8.block3);
        arrayIndex++;
        translated[arrayIndex] = base8Symbol(temp.base8.block2);
        arrayIndex++;
        translated[arrayIndex] = base8Symbol(temp.base8.block1);
        arrayIndex++;
        translated[arrayIndex] = ' ';
        arrayIndex++;
        translated[arrayIndex] = base8Symbol(codewords.array[i].base8.block5);
        arrayIndex++;
        translated[arrayIndex] = base8Symbol(codewords.array[i].base8.block4);
        arrayIndex++;
        translated[arrayIndex] = base8Symbol(codewords.array[i].base8.block3);
        arrayIndex++;
        translated[arrayIndex] = base8Symbol(codewords.array[i].base8.block2);
        arrayIndex++;
        translated[arrayIndex] = base8Symbol(codewords.array[i].base8.block1);
        arrayIndex++;
        translated[arrayIndex] = '\n';
        arrayIndex++;


        i++;
        address++;
    }
    i=0;
    while(i<datawords.numberOfWords){
        Word temp = createInstanceOfWord();
        /*Address format*/
        temp.fullword.cell = address;
        translated[arrayIndex] = base8Symbol(temp.base8.block4);
        arrayIndex++;
        translated[arrayIndex] = base8Symbol(temp.base8.block3);
        arrayIndex++;
        translated[arrayIndex] = base8Symbol(temp.base8.block2);
        arrayIndex++;
        translated[arrayIndex] = base8Symbol(temp.base8.block1);
        arrayIndex++;
        translated[arrayIndex] = ' ';
        arrayIndex++;
        translated[arrayIndex] = base8Symbol(datawords.array[i].base8.block5);
        arrayIndex++;
        translated[arrayIndex] = base8Symbol(datawords.array[i].base8.block4);
        arrayIndex++;
        translated[arrayIndex] = base8Symbol(datawords.array[i].base8.block3);
        arrayIndex++;
        translated[arrayIndex] = base8Symbol(datawords.array[i].base8.block2);
        arrayIndex++;
        translated[arrayIndex] = base8Symbol(datawords.array[i].base8.block1);
        arrayIndex++;
        translated[arrayIndex] = '\n';
        arrayIndex++;

        i++;
        address++;
    }
    return translated;
}
char base8Symbol(int number){
    /* !@#$%^&* is assigned to 01234567 */
    switch (number){
        case 0:
            return '!';
        case 1:
            return '@';
        case 2:
            return '#';
        case 3:
            return '$';
        case 4:
            return '%';
        case 5:
            return '^';
        case 6:
            return '&';
        case 7:
            return '*';
        default:
            return NULL;
    }
}
int freeAll(){
    int i = 0;
    while(i<symbols.numberOfSymbols){
        free(symbols.array[i].name);
        symbols.array[i].name = NULL;
        i++;
    }
    free(symbols.array);
    free(codewords.array);
    free(datawords.array);
    symbols.array = NULL;
    symbols.size = 0;
    symbols.numberOfSymbols = 0;
    codewords.array = NULL;
    codewords.size = 0;
    codewords.numberOfWords = 0;
    datawords.array = NULL;
    datawords.numberOfWords = 0;
    datawords.size = 0;
}