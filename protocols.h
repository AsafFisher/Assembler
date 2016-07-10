#include <string.h>

typedef union Word{
	/*Binary structure: 15 bits basic cell*/
	struct{
		unsigned int cell:15;
	}word;

	/*Binary structure: |DUMY|GRP|OPCODE|SRCAR|DESTAR|ERA|*/
	struct{
		unsigned int ERA:2;
		unsigned int destar:2;
		unsigned int srcar:2;
		unsigned int opcode:4;
		unsigned int grp:2;
		unsigned int dumy:3;
	}command;
	/*Binary structure: |DUMY|PARAM2|PARAM1|ERA|*/
	struct paddress{
		unsigned int ERA:2;
		int src:6;
		int dest:6;
		unsigned int dumy:1;

	}paddress;
	/*Binary structure: |VALUE|ERA|*/
	struct pvalue{
		unsigned int ERA:2;
		signed int value:13;
	}pvalue;

	struct base8{
		unsigned int block1:3;
		unsigned int block2:3;
		unsigned int block3:3;
		unsigned int block4:3;
		unsigned int block5:3;
	}base8;

}Word;
typedef struct{
	Word *array;
	int numberOfWords;
	int size;
}Words;

/*Address resolution*/
enum{INSTANT_ADDRESS_RESOLUTION = 0,DIRECT_ADDRESS_RESOLUTION, INSTANT_DYNAMIC_ADDRESS_RESOLUTION, DIRECT_REGISTER_ADDRESS_RESOLUTION, NONE = -1};
/*ERA type.*/
enum{ABSULUT=0,EXTERNAL,RELOCATABLE};
/*Group*/
enum{NOP = 0, ONEOP,TWOP};
/*Action opcode*/
enum{mov=0,cmp,add,sub,nt,clr,lea,inc,dec,jmp,bne,red,prn,jsr,rts,stop};
Word createInstanceOfWordWithParam(Word word);
Word createInstanceOfWord();
Word createInstaceOfCommandWithParam(int pERA,int pdestar,int psrcar,int popcode,int pgrp);
Word createInstaceOfCommand(int action);
Word createInstaceOfParameterAdrressWithParam(int pERA,int param1,int param2);
Word createInstaceOfParameterAdrress();
Word createInstaceOfParameterValueWithParam(int pERA, int pvalue);
Word createInstaceOfParameterValue();
int setWordValue(Word *word, int value);
int checkSize(Words *words);
