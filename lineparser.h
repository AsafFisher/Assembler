#include "protocols.h"
int IC;
int DC;

void parseLine(char* line);

char actions[] = {"mov","cmp","add","sub","no","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop"};
typedef Lable enum{ACTION = 0,INSTRUCTION}Type;
typedef symboleTable struct{
  char* name;
  int address;
  int isExternal;
  Type type;

}
