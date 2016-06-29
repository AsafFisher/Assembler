#include "protocols.h"
int IC;
int DC;
int parseLine(char* line);


typedef enum{ACTION = 0,INSTRUCTION}Type;
typedef struct{
  char* name;
  Word address;
  int isExternal;
  Type type;

}Symbole;
typedef struct{
  Symbole* array;
  int numberOfSymboles;
  int size;
}Symboles;
