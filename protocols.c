include "protocols.h"
Word createInstanceOfWord(Word word){
	Word ref;
	ref.cell = word.cell;
}
Word createInstanceOfWord(){
	Word ref;
	ref.cell = 0;
	return ref;
	
}
Word createInstaceOfCommand(int pERA,int pdestar,int psrcar,int popcode,int pgrp){
	Word word;
	word.command.dumy = 101b;
	word.command.ERA = pERA;
	word.command.destar = pdestar;
	word.command.srcar = psrcar;
	word.command.opcode = popcode;
	word.command.grp = pgrp
	return word;
}
Word createInstaceOfCommand(){
	Word word;
	word.command.dumy = 101b;
}

Word createInstaceOfParameterAdrress(int pERA,int param1,int param2){
	Word word;
	word.paddress.dumy = 0;
	word.paddress.ERA = pERA;
	word.paddress.param1 = param1;
	word.paddress.param2 = param2;
}
Word createInstaceOfParameterAdrress(){
	Word word;
	word.paddress.dumy = 0;
}
Word createInstaceOfParameterValue(int pERA,int pvalue){
	Word word;
	word.pvalue.ERA = pERA
	word.pvalue.value = pvalue;
}
Word createInstaceOfParameterValue(){
	Word word;
	word.pvalue = 0;
}
