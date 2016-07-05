#include "protocols.h"
#include <stdio.h>
#include <stdlib.h>
#define DUMMY 5
#define REGISTERS_NUMER 8
char* registers[] = {"‫‪r0","r1","r2","r3","r4","r5","r6","r7‬‬"};
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

	}ommand.srcar
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

int setWordValue(Word *word, int value){
	printf("TESTWORKS");
	if(value>((1<<16) - 1)){
		printf("Value:'%d'is too big.\n",(int)word->word.cell );
		return 0;
	}
	word->word.cell = value;
	return 1;
}

int checkSize(Words *words){
	if (words->size<=words->numberOfWords) {
    /* code */

    words->size += 10;
    /*WARNING: DONT REALLOC DIRECTLY TO SAME PARAM!-----------------------------------------*/
    if ((words->array = realloc(words->array, sizeof(Word)*words->size))==NULL) {
      /* code */
      printf("> ERROR NOT ENOGH SPACE!!!\n");
			return 0;
    }
    printf("> ARRAY RESIZED!\n");
		return 1;
  }
	return 1;

}
int setUpCommandParams(Word* word,char *token){
	char *temp = token;
	switch (word.command.opcode) {
		case cmp:
		if (token = strtok(NULL," ,")==NULL) {
			printf("> ERROR '%s' bad token.\n",temp );
		}
/*TODO: Comtinue...-----------------------------------*/
		while (i < REGISTERS_NUMER) {
	    if (!strcmp(registers[i],token)) {
				word.command.srcar =
	      return 1;
	    }
		break;
		case move:
		case add:
		case sub:

		break;
		case nt:
		case clr:
		case inc:
		case dec:
		case jmp:
		case bne:
		case red:
		case jsr
		break;
		case prn:
		break;
		case lea:
		break;

	}
}
