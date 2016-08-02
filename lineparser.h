#include "protocols.h"
/*Instruction counter*/
/*int IC;*/

/*Data counter*/
/*int DC;*/
int parseLine(char* line, int lineNumber);
void printArr(void);
void printUndefineds(void);


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
  char* name;
  int* shows;
  int numberOfShows;
  int size;
}Undefined;
typedef struct{
  Undefined* array;
  int numberOfUnd;
  int size;
}Undefineds;
