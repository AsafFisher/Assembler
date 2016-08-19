#ifndef LINEPARSER_H
#define LINEPARSER_H
#define LINE_MAX 80
#include "protocols.h"
int parseLine(char* line, int lineNumber,FILE* err);
/*Link variables to their values.*/
int variableLinker(FILE *file);
void printArr(void);
/*Prints symbols array*/
void printSymbols(void);
/*return the base8 form of the instructions and data*/
char* convertToBase8();
/*Free all the variables*/
int freeAll(void);
/*Allocate extern file*/
void externFile();
/*return the string of the extern file.*/
char* getExterns();
/*return the string of entry file.*/
char* getEntrys();


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