typedef Word struct{
	unsigned int cell:15
}

typedef union Command{
	struct{

		unsigned int ERA:2
		unsigned int destar:2
		unsigned int srcar:2
		unsigned int opcode:4
		unsigned int grp:2
		unsigned int dumy:3
	} cmd
	Word word;
}
typedef union Parameter{
	struct{
		
	}
	Word word;
}


enum{INSTANT_ADRESS_RESOLUTION = 0,DIRECT_ADRESS_RESOLUTION,INSTANT_ADRESS_RESOLUTION,REGISTER_DIRECT_ADRESS_RESOLUTION}
enum{ABSULUT,}
void setWord(Word word, int number);
void setCommand(Word word)
Command createInstaceOfCommand(int pERA,int pdestar,int psrcar,int popcode,int pgrp);
Command createInstaceOfCommand();
