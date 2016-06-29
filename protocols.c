#include "protocols.h"
#define DUMMY 5
Word createInstanceOfWordWithParam(Word word){
	Word ref;
	ref.word.cell = word.word.cell;
	return ref;
}
Word createInstanceOfWord(){
	Word ref;
	ref.word.cell = 0;
	return ref;

}
Word createInstaceOfCommandWithParam(int pERA,int pdestar,int psrcar,int popcode,int pgrp){
	Word word;
	/*101 b = 5*/
	word.command.dumy = DUMMY;
	word.command.ERA = pERA;
	word.command.destar = pdestar;
	word.command.srcar = psrcar;
	word.command.opcode = popcode;
	word.command.grp = pgrp;
	return word;
}
Word createInstaceOfCommand(int action){
	Word word;
	switch (action) {
		case mov:
		word.command.grp = TWOP;
		break;
		case cmp:
		word.command.grp = TWOP;
		break;
		case add:
		word.command.grp = TWOP;
		break;
		case sub:
		word.command.grp = TWOP;
		break;
		case nt:
		word.command.grp = ONEOP;
		break;
		case clr:
		word.command.grp = ONEOP;
		break;
		case lea:
		word.command.grp = TWOP;
		break;
		case inc:
		word.command.grp = ONEOP;
		break;
		case dec:
		word.command.grp = ONEOP;
		break;
		case jmp:
		word.command.grp = ONEOP;
		break;
		case bne:
		word.command.grp = ONEOP;
		break;
		case red:
		word.command.grp = ONEOP;
		break;
		case prn:
		word.command.grp = ONEOP;
		break;
		case jsr:
		word.command.grp = ONEOP;
		break;
		case rts:
		word.command.grp = NOP;
		break;
		case stop:
		word.command.grp = NOP;
		break;

	}
	word.command.opcode = action;
	word.command.dumy = DUMMY;
	return word;
}

Word createInstaceOfParameterAdrressWithParam(int pERA,int src,int dest){
	Word word;
	word.paddress.dumy = 0;
	word.paddress.ERA = pERA;
	word.paddress.src = src;
	word.paddress.dest = dest;
	return word;
}
Word createInstaceOfParameterAdrress(){
	Word word;
	word.paddress.dumy = 0;
	return word;
}
Word createInstaceOfParameterValueWithParam(int pERA,int pvalue){
	Word word;
	word.pvalue.ERA = pERA;
	word.pvalue.value = pvalue;
	return word;
}
Word createInstaceOfParameterValue(){
	Word word;
	word.pvalue.value = 0;
	return word;
}
