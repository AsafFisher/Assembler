#include "protocols.h"
int IC;
int DC;

void parseLine(char* line);
char actions[] = {"mov","cmp","add","sub","no","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop"};
//TODO:FINISH THIS!!
typedef  enum{ACTION = 0,INSTRUCTION}
typedef symboleTable struct{
  char* name;
  int address;
  int isExternal;




}
