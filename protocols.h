s
typedef Command struct{
	unsigned int ERA:2
	unsigned int destar:2
	unsigned int srcar:2
	unsigned int opcode:4
	unsigned int grp:2
	unsigned int dumy:3
}
typedef Word struct{
	unsigned int cell:15
}
void setWord(Word word, int number);
void setCommand(Word word)
Command createInstaceOfCommand(int pERA,int pdestar,int psrcar,int popcode,int pgrp);
Command createInstaceOfCommand();

