#ifndef LINEPARSER_H
#define LINEPARSER_H
#define LINE_MAX 80
#include "protocols.h"
int parseLine(char* line, int lineNumber);
int variableLinker(FILE *file);
void printArr(void);
void printUndefineds(void);
void printSymbols(void);


typedef enum{ACTIONT = 0,INSTRUCTION}Type;
typedef struct{
  char* name;
  Word address;
  int isExternal;
  Type type;

}Symbol;
typedef struct{
  Symbol* array;
  int numberOfSymbols;
  int size;
}Symbols;

typedef struct{
    int lineNumber;
    unsigned int type;
}Show;
typedef struct{
  char* name;
  Show* shows;
  int numberOfShows;
  int size;
}Undefined;
typedef struct{
  Undefined* array;
  int numberOfUnd;
  int size;
}Undefineds;
#endif