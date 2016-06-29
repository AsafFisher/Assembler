int IC;
int DC;

void parseLine(char* line);


typedef enum{ACTION = 0,INSTRUCTION}Type;
typedef struct{
  char* name;
  int address;
  int isExternal;
  Type type;

}Symbole;
