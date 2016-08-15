#ifndef LINEPARSER_H
#define LINEPARSER_H
#define LINE_MAX 80
#include "protocols.h"
int parseLine(char* line, int lineNumber);
int variableLinker(FILE *file);
void printArr(void);
void printSymbols(void);
char* convertToBase8();
int freeAll(void);

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

#endif