

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
	} command;
	/*Binary structure: |DUMY|PARAM2|PARAM1|ERA|*/
	struct{
		unsigned int ERA:2;
		unsigned int src:6;
		unsigned int dest:6;
		unsigned int dumy:1;
		
	}paddress;
	/*Binary structure: |VALUE|ERA|*/
	struct{
		unsigned int ERA:2;
		unsigned int value:13;
	}pvalue
	
	struct{
		unsigned int block1:3;
		unsigned int block2:3;
		unsigned int block3:3;
		unsigned int block4:3;
		unsigned int block5:3;
	}base8;
	
}


enum{INSTANT_ADRESS_RESOLUTION = 0,DIRECT_ADRESS_RESOLUTION,INSTANT_ADRESS_RESOLUTION,REGISTER_DIRECT_ADRESS_RESOLUTION}
enum{ABSULUT,}
Word createInstanceOfWord(Word word);
Word createInstanceOfWord();
Word createInstaceOfCommand(int pERA,int pdestar,int psrcar,int popcode,int pgrp);
Word createInstaceOfCommand();
Word createInstaceOfParameterAdrress(int pERA,int param1,int param2);
Word createInstaceOfParameterAdrress();
Word createInstaceOfParameterValue(int value);
Word createInstaceOfParameterValue();

