
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "lineparser.h"
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
FILE* errorout;
int lineNumber;
char* externs;
int externsPtr = 0;
char* entrys;
int entryPtr = 0;

/*Add symbol to the symbol array*/
int addSymbol(Symbol symbol);

/*Set the symbol address to the location*/
int setSymbolValue(char *line, Word *location, int commandIndex);

/*Update data to the array (.string data and .data data)*/
int updateDataToMemory(char *token);

/*update .string data to array*/
int updateStringToMemory(char *token);

/*Update parameters of command to memory*/
int updateCommandParamToMemory(Word command, char *token);

/*set up the command's word parameters.*/
int setUpCommandParams(Word *word, char *token);

/*identify a with idar type*/
int identifyIDARSymbol(char* idarStr,Word *location);

/*Take a base8 number and convert it into a special base8 char*/
char base8Symbol(int number);


/*Release memory allocations*/
int freeAll();

/*Return a spesific symbol from the symbol array using its name.*/
Symbol* getSymbolByName(char* param);

/*Crop the value from startBit to endBit in a Word type.*/
Word cropRangeFromValue(Word value, int startBit, int endBit);
/*Check if param is INSTANT_ADDRESS_RESOLUTION, create a Word and save space in memory.
* location is the type of the assignment (SOURCE OR DESTINATION).
*/
int isParamIAR(Word *command, char *token, int location);

/*Check if param is DIRECT_REGISTER_ADDRESS_RESOLUTION, create a Word and save space in memory.*/
int isParamDRAR(Word *command, char *token, int location);

/*Check if param is INSTANT_DYNAMIC_ADDRESS_RESOLUTION, create a Word and save space in memory.*/
int isParamIDAR(Word *command, char *token, int location);

/*Check array size and rellocating it.*/
int checkSymbolSize();







int parseLine(char *buff, int number, FILE* err) {
    Symbol symbol;
    char *token;
    int hasSymbol;
    int actionID = 0;
    int wasFound;
    Word word;
    hasSymbol = false;
    lineNumber = number;
    errorout = err;
    Word *warningsRemover = malloc(sizeof(Word));
    warningsRemover->pvalue.ERA = 0;
    warningsRemover->paddress.ERA = 0;
    warningsRemover->paddress.dumy = 0;
    free(warningsRemover);


    if (!checkSize(&codewords)) {
        /* ERROR ALLOCATING SPACE! */
        return 0;
    }
    if (!checkSize(&datawords)) {
        /* ERROR ALLOCATING SPACE! */
        return 0;
    }
	/*Check if line is legal.*/
	if(!is_legal_line( buff,lineNumber))
		return 0;
    /*Using strtok to parse the text.*/
    if (!(token = strtok(buff, " \n"))) {
        return 1;
    }
    /*Check for label*/
    if (token[strlen(token) - 1] == ':') {

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
            fprintf(errorout,"> Warning: label not needed. LINE: %d",lineNumber);
        }
        /*Add external symbol.*/
        if ((token = strtok(NULL, "\n")) != NULL) {
            symbol.name = malloc(strlen(token) + 1);
            strcpy(symbol.name, token);
            symbol.type = INSTRUCTION;
            symbol.address.pvalue.value = 0;
            symbol.isExternal = true;
            if(!addSymbol(symbol)){
                return 0;
            }
            return 1;
        }


    }

    if (!strcmp(token, ENTRY)) {
        return 1;
    }

    /*Check if line is command line*/
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
        /* Check if action was found*/
        int iaction = 0;
        while (iaction < ACTION_NUMBER) {
            if (!strcmp(actions[iaction], token)) {
                wasFound = true;
                actionID = iaction;
                break;
            }
            iaction++;
        }
    }
    if (!wasFound) {
        fprintf(errorout,"> ERROR (LINE %d) operator: '%s' Was not found! \n", lineNumber, token);
        return 0;
    }
    /*Check size and reallocating the array*/
    if (!checkSize(&codewords)) {
        /* ERROR ALLOCATING SPACE! */
        return 0;
    }

    word = createInstaceOfCommand((unsigned int) actionID);
    /*codewords.array[IC] = word;*/


/*Check if parameter exist.*/
    {
        char *temp = strtok(NULL, "\n\t");
        if (temp == NULL && word.command.grp != NOP) {
            /*ERROR NO PARAMETERS*/
            fprintf(errorout,"> ERROR NO PARAMETER! (LINE: %d)\n",lineNumber);
            return 0;
        }
        token = temp;
    }

    if (!updateCommandParamToMemory(word, token)) {
        /*ERROR: invalid parameter*/
        return 0;
    }


    return 1;

}
int variableLinker(FILE *input) {
    /*The acctual index of the command*/
    int commandIndex = 0;
    /*The line of the file.*/

    int currentFileLine = 0;

    entrys = calloc((size_t)LINE_MAX*(symbols.numberOfSymbols),sizeof(char));
    /*Go over each command in the code array*/
    while (commandIndex < codewords.numberOfWords) {
        /*The current Word*/
        Word *currentWord = codewords.array + commandIndex;
        /*The actual line of the command*/
        int currentCommandLine = codewords.lines[commandIndex];
        char line[LINE_MAX];
        char help[LINE_MAX];
        char *temp = NULL;
        /*This loop makes sure that each time the pointer of the file points to the same line of the specific command.*/
        while(currentFileLine<currentCommandLine){
            char *find;
            if(fgets(line,LINE_MAX,input)==NULL){
                fprintf(errorout,"> ERROR file corrupted.");
                return 0;
            }
            strcpy(help,line);
            /*Using strtok to parse the text.*/
            if (!(find = strtok(help, " \n"))) {
                return 1;
            }
            /*Check for label*/
            if (find[strlen(find) - 1] == ':') {
                find = strtok(NULL, " \n");
            }
            if (!strcmp(find, ENTRY)) {
                /*Add external symbol.*/
                if ((find = strtok(NULL, "\n")) != NULL) {
                    int entrindex = 0;
                    while (entrindex<symbols.numberOfSymbols){
                        if (!strcmp(find,symbols.array[entrindex].name)){
                            Word address = createInstanceOfWord();
                            int len = 0;
                            while(len<strlen(find)){
                                entrys[entryPtr] = find[len];
                                len++;
                                entryPtr++;
                            }
                            entrys[entryPtr] = ' ';
                            entryPtr++;
                            if(symbols.array[entrindex].type == INSTRUCTION){
                                address.fullword.cell = (unsigned int)symbols.array[entrindex].address.pvalue.value+MEMORY_START+codewords.numberOfWords;
                            }else {
                                address.fullword.cell = (unsigned int) symbols.array[entrindex].address.pvalue.value + MEMORY_START;
                            }
                            entrys[entryPtr] = base8Symbol(address.base8.block4);
                            entryPtr++;
                            entrys[entryPtr] = base8Symbol(address.base8.block3);
                            entryPtr++;
                            entrys[entryPtr] = base8Symbol(address.base8.block2);
                            entryPtr++;
                            entrys[entryPtr] = base8Symbol(address.base8.block1);
                            entryPtr++;
                            entrys[entryPtr] = '\n';
                            entryPtr++;
                            break;



                        }
                        entrindex++;
                    }
                }


            }
            currentFileLine++;
        }

        /*Text manipulations*/
        if(strstr(line,":")!=NULL)
            temp = strtok(line, ":");
        if(temp == NULL){
            temp = strtok(line, "\n");
        }else{
            temp = strtok(NULL, "\n");
        }
        /*Sort each command using its number of parameters.*/
        if (currentWord->command.grp == NOP) {
            /*No operators next command is right at commandIndex+1*/
            commandIndex++;
            continue;
        }
        if (currentWord->command.grp == ONEOP) {
            /*One operator, variable is located at commandIndex+1 next command is right at commandIndex+2*/
            int varIndex = commandIndex + 1;
            strtok(temp, " ");
            temp = strtok(NULL," \n");

            /*Replace the empty cell of the symbols with the actual address. (Direct address resolution and instant dynamic address resolution)*/
            switch(currentWord->command.destar){
                case DIRECT_ADDRESS_RESOLUTION:
                    /*Search in the */
                    if(!setSymbolValue(temp, &codewords.array[varIndex], varIndex)){
                        fprintf(errorout,"> To check error location please go to line: %d.\n",currentCommandLine);
                        return 0;
                    }
                    break;
                case INSTANT_DYNAMIC_ADDRESS_RESOLUTION:
                    if(!identifyIDARSymbol(temp,&codewords.array[varIndex])){
                        fprintf(errorout,"> To check error location please go to line: %d.\n",currentCommandLine);
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
                    if(!setSymbolValue(temp, &codewords.array[firstVarIndex], firstVarIndex)){
                        fprintf(errorout,"> To check error location please go to line: %d.\n",currentCommandLine);
                        return 0;
                    }
                    break;
                case INSTANT_DYNAMIC_ADDRESS_RESOLUTION:
                    if(!identifyIDARSymbol(temp,&codewords.array[firstVarIndex])){
                        fprintf(errorout,"> To check error location please go to line: %d.\n",currentCommandLine);
                        return 0;
                    }
                    break;
                default:
                    break;
            }
            temp = strtok(NULL," ,\n");
            /*Destenation*/
            switch(currentWord->command.destar){
                case DIRECT_ADDRESS_RESOLUTION:
                    if(!setSymbolValue(temp, &codewords.array[secondVarIndex], secondVarIndex)){
                        fprintf(errorout,"> To check error location please go to line: %d.\n",currentCommandLine);
                        return 0;
                    }
                    break;
                case INSTANT_DYNAMIC_ADDRESS_RESOLUTION:
                    if(!identifyIDARSymbol(temp,&codewords.array[secondVarIndex])){
                        fprintf(errorout,"> To check error location please go to line: %d.\n",currentCommandLine);
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
    externs[externsPtr] = 0;
    externsPtr++;
    entrys[entryPtr] = 0;
    entryPtr++;
    return 1;

}


/*Gets a parameter and a Word and replace the contained in the word with the Address of the symbol (return 0 if symbol does not exist)*/
int setSymbolValue(char *param, Word *location, int commandIndex) {
    Symbol* symbol;
    if((symbol = getSymbolByName(param))==NULL){
        fprintf(errorout,"> ERROR: variable named '%s' was never declared!\n",param);
        return 0;
    }
    if(symbol->isExternal){
        Word address = createInstanceOfWord();
        address.fullword.cell = (unsigned int)commandIndex+MEMORY_START;
        symbol->address.pvalue.ERA = EXTERNAL;
        location->fullword.cell = symbol->address.fullword.cell;
        int strindex = 0;
        while(strindex<strlen(symbol->name)){
            externs[externsPtr] = symbol->name[strindex];
            externsPtr++;
            strindex++;
        }
        externs[externsPtr] = ' ';
        externsPtr++;
        externs[externsPtr] = base8Symbol(address.base8.block4);
        externsPtr++;
        externs[externsPtr] = base8Symbol(address.base8.block3);
        externsPtr++;
        externs[externsPtr] = base8Symbol(address.base8.block2);
        externsPtr++;
        externs[externsPtr] = base8Symbol(address.base8.block1);
        externsPtr++;
        externs[externsPtr] = '\n';
        externsPtr++;



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
    /*Resize array if needed.*/
    if (!checkSymbolSize()) {
        return 0;
    }

    /*Check if symbol already exist.*/
    while (i < symbols.numberOfSymbols) {
        if (!strcmp(symbols.array[i].name, symbol.name)) {
            /*-------------------ERROR: Multiple declaration!----------------------*/
            fprintf(errorout,"> ERROR: duplicate declaration: %s\n", symbol.name);
            return 0;
        }
        i++;
    }
    symbols.array[symbols.numberOfSymbols] = symbol;
    symbols.numberOfSymbols++;
    return 1;
}

/*Update int values to the memory (.data values)*/
int updateDataToMemory(char *token) {
    Word value;
    token = strtok(token, ", \n");
    while (token != NULL) {
        long digitVal;
        int i = 0;

        while (i < strlen(token)) {
            /* Check validation */
            if (!(isdigit(token[i])) && (token[i] != '-') && (token[i] != '+')) {
                /* code */
                fprintf(errorout,"> ERROR: '%s' is invalid value!\n", token);
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
        if (!setWordValue(&value, (unsigned int) digitVal, NULL)) {
            /*ERROR VALUE TOO BIG*/
            return 0;
        }
        datawords.array[datawords.numberOfWords] = value;
        token = strtok(NULL, ", \n");
        datawords.numberOfWords++;
    }
    return 1;
}

int updateStringToMemory(char *token) {
    /*
    Detect char -> ""
    Check each letter t'ill you get to "
    */
    /*index of start of string*/
    int i = 0;
    int lindex;
    int found = 0;

    while (i < (strlen(token) - 1)) {
        if (token[i] == '\"') {
            /*found last quotes*/
            found = 1;
            break;
        }
        i++;
    }
    if (!found) {
        /*ERROR last quote not found!*/
        fprintf(errorout,"> ERROR missing first -> Cytation <-\n");
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
        fprintf(errorout,"> ERROR missing last -> Cytation <-\n");
        return 0;
    }
    while (i < lindex) {
        /*Parse the inner string.*/
        Word val;
        if (!checkSize(&datawords)) {
            /* ERROR ALLOCATING SPACE! */

            return 0;
        }
        if (!setWordValue(&val, (unsigned int) token[i],errorout)) {
            /*ERROR VALUE TOO BIG*/
            return 0;
        }
        datawords.array[datawords.numberOfWords] = val;
        i++;
        datawords.numberOfWords++;
    }
    Word val;
    if(!setWordValue(&val, 0, errorout)){
        return  0;
    }
    datawords.array[datawords.numberOfWords] = val;
    datawords.numberOfWords++;


    return 1;
}

int updateCommandParamToMemory(Word command, char *token) {
    int error = 0;
    switch (command.command.grp) {
        case NOP:
            codewords.array[codewords.numberOfWords] = command;
            codewords.lines[codewords.numberOfWords] = lineNumber;
            codewords.numberOfWords++;

            return 1;
        case ONEOP: {
            int i = 0;
            while (i < strlen(token)) {
                if (token[i] == ',') {
                    /* ERROR TOO MANY PARAMS */
                    fprintf(errorout,"> ERROR (LINE: %d): \"%s\" is an iligal parameter for this type of command.\n",lineNumber, token);
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
            codewords.numberOfWords++;
            if (!setUpCommandParams(&codewords.array[codewords.numberOfWords - 1], token)) {
                fprintf(errorout,"> ERROR (LINE: %d)\n",lineNumber);
                return 0;
            }

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
                fprintf(errorout,">ERROR (LINE: %d): Missing parameter: %s\n",lineNumber, token);
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
            codewords.numberOfWords++;
            if (!setUpCommandParams(&codewords.array[codewords.numberOfWords - 1], token)) {
                fprintf(errorout,"> ERROR (LINE: %d)\n",lineNumber);
                return 0;
            }


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
                fprintf(errorout,"> ERROR (LINE: %d):'%s'\n",lineNumber, token);
                return 0;
            }
            printf("TOKEN %s\n", token);
            if (strlen(token) == 0) {
                fprintf(errorout,"ERROR (LINE: %d): Token has no arguments!\n",lineNumber);
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
                fprintf(errorout,"> ERROR (LINE: %d): '%s'\n", lineNumber,token);
                ERROR = 1;
                return !ERROR;
            }
            if (strlen(token) == 0) {
                fprintf(errorout,"> ERROR: Token has no arguments! LINE: %d\n",lineNumber);
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
            /*FIRST PARAM: ------------*/

            if ((token = strtok(token, " ,")) == NULL) {
                fprintf(errorout,"> ERROR (LINE: %d): '%s'\n", lineNumber,token);
                return 0;
            }
            if (strlen(token) == 0) {
                fprintf(errorout,"ERROR (LINE %d): Token has no arguments!\n",lineNumber);
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
                fprintf(errorout,"> ERROR 537: '%s'LINE: %d\n", token,lineNumber);
                ERROR = 1;
                return !ERROR;
            }
            if (strlen(token) == 0) {
                fprintf(errorout,"> ERROR: Token has no arguments! LINE: %d\n",lineNumber);
                ERROR = 1;
                return !ERROR;
            }
            if (isParamIAR(command, token, DESTINATION)) {
                fprintf(errorout,"Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
                ERROR = 1;
            } else if (isParamDRAR(command, token, DESTINATION)) {
                ERROR = 0;
            } else if (isParamIDAR(command, token, DESTINATION)) {
                fprintf(errorout,"Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
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
                fprintf(errorout,"> ERROR (LINE: %d): '%s'\n", lineNumber,token);
                ERROR = 1;
                return !ERROR;
            }
            if (strlen(token) == 0) {
                fprintf(errorout,"> ERROR (LINE: %d): Token has no arguments!\n",lineNumber);
                ERROR = 1;
                return !ERROR;
            }
            if (isParamIAR(command, token, DESTINATION)) {
                fprintf(errorout,"Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
                ERROR = 1;
            } else if (isParamDRAR(command, token, DESTINATION)) {
                ERROR = 0;
            } else if (isParamIDAR(command, token, DESTINATION)) {
                fprintf(errorout,"Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
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
                fprintf(errorout,"> ERROR (LINE: %d): '%s'\n", lineNumber,token);
                ERROR = 1;
                return !ERROR;
            }
            if (strlen(token) == 0) {
                fprintf(errorout,"> ERROR (LINE: %d): Token has no arguments!\n",lineNumber);
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
                fprintf(errorout,"> ERROR (LINE: %d): '%s'\n", lineNumber,token);
                return 0;
            }
            if (strlen(token) == 0) {
                fprintf(errorout,"ERROR (LINE: %d): Token has no arguments!\n",lineNumber);
                return 0;
            }
            if (isParamIAR(command, token, SOURCE)) {
                fprintf(errorout,"Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
                ERROR = 1;
            } else if (isParamDRAR(command, token, SOURCE)) {
                fprintf(errorout,"Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
                ERROR = 1;
            } else if (isParamIDAR(command, token, SOURCE)) {
                fprintf(errorout,"Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
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
                fprintf(errorout,"> ERROR (LINE: %d): Token has no arguments!\n",lineNumber);
                ERROR = 1;
                return !ERROR;
            }
            if (isParamIAR(command, token, DESTINATION)) {
                fprintf(errorout,"Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
                ERROR = 1;
            } else if (isParamDRAR(command, token, DESTINATION)) {
                ERROR = 0;
            } else if (isParamIDAR(command, token, DESTINATION)) {
                fprintf(errorout,"Error cannot do command with parameter: '%s' LINE: %d\n",token,lineNumber);
                ERROR = 1;
            } else {
                command->command.destar = DIRECT_ADDRESS_RESOLUTION;
                arg.paddress.ERA = RELOCATABLE;
                codewords.array[codewords.numberOfWords] = arg;
                codewords.numberOfWords++;
            }
            return !ERROR;
        default:
            fprintf(errorout,"> ERROR (LINE: %d) Wrong Abort.\n",lineNumber);
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
    arg.paddress.ERA = ABSULUT;
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
    /*Check IDAR Template*/
    while (i < strlen(token)) {
        if (structureIndex >= NUMBEROFSTRUCTURE) {
            break;
        }
        if (token[i] == idarStructure[structureIndex]) {
            if (structureIndex == 0) {
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
    int endOfName = 0;
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
    memcpy(symbolName,idarStr,sizeof(char)*endOfName);
    symbol = getSymbolByName(symbolName);
    if(symbol == NULL){
        fprintf(errorout,"> ERROR (LINE: %d): variable named '%s' was never declared!\n",lineNumber,symbolName);
        return 0;
    }
    switch (symbol->type){
        case INSTRUCTION:
            if(from<0&&from>=13){
                fprintf(errorout,"> ERROR (LINE: %d): The value '%d' must be between 0 to 12 ",lineNumber,from);
                return 0;
            }
            if(to<0&&to>=13){
                fprintf(errorout,"> ERROR (LINE: %d): The value '%d' must be between 0 to 12 ",lineNumber,to);
                return 0;
            }
            value = cropRangeFromValue(datawords.array[symbol->address.pvalue.value], from, to);
            location->fullword.cell = value.fullword.cell;
            return 1;
        case ACTIONT:
            if(from<0&&from>=13){
                fprintf(errorout,"> ERROR (LINE: %d): The value '%d' must be between 0 to 12 ",lineNumber,from);
                return 0;
            }
            if(to<0&&to>=13){
                fprintf(errorout,"> ERROR (LINE: %d): The value '%d' must be between 0 to 12 ",lineNumber,to);
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
            fprintf(errorout,"> ERROR! not enogh space\n");
            return 0;
        }

    }
    return 1;
}

Word cropRangeFromValue(Word value, int startBit, int endBit){
    /*Crop the range of bits and complete the MSB*/
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
        printf("The Symbol: '%s' has an address of: '%u' isExternal = '%d' Type: '%d' ", symbols.array[i].name,
               symbols.array[i].address.fullword.cell, symbols.array[i].isExternal, symbols.array[i].type);
        printf("\n");
    }
    printf("--------------------------------------\n");
}

void printArr() {
    printInstructionsArray(&codewords);
}
void externFile(){
    externs = calloc((size_t)LINE_MAX*(symbols.numberOfSymbols),sizeof(char));
}
char* getExterns(){
    if(strlen(externs)==0) {
        free(externs);
        return NULL;
    }
    return externs;
}
char* getEntrys(){
    if(strlen(entrys)==0){
        free(entrys);
        return NULL;
    }
    return entrys;

}
char* convertToBase8(){
    char* translated = calloc(LINE_MAX*(codewords.numberOfWords+datawords.numberOfWords),sizeof(char));
    int arrayIndex = 0;
    int i = 0;
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
    externs = NULL;
    externsPtr = 0;
    entrys = NULL;
    externsPtr = 0;
    return 1;
}