#include "protocols.h"
#include <stdio.h>
#include <stdlib.h>
#define DUMMY 5
Word createInstanceOfWordWithParam(Word word){
	Word ref;
	ref.fullword.cell = word.fullword.cell;
	return ref;
}
Word createInstanceOfWord(){
	Word ref;
	ref.fullword.cell = 0;
	return ref;

}
Word createInstaceOfCommandWithParam(unsigned int pERA,unsigned int pdestar,unsigned int psrcar,unsigned int popcode,unsigned int pgrp){
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
Word createInstaceOfCommand(unsigned int action){
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
		default:
			word.command.grp = NOP;
			break;

	}
	word.command.opcode = action;
	word.command.dumy = DUMMY;
	return word;
}

Word createInstaceOfParameterAdrressWithParam(unsigned int pERA, unsigned int src, unsigned int dest){
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
Word createInstaceOfParameterValueWithParam(unsigned int pERA, unsigned int pvalue){
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

int setWordValue(Word *word, unsigned int value){
	if(value>((1<<16) - 1)&&(int)value>=0){
		printf("Value: '%d' is too big.\n",value );
		return 0;
	}
	word->fullword.cell = value;
    printf("TEST: %d\n",word->fullword.cell);
	return 1;
}

int checkSize(Words *words){
	if (words->size<=(words->numberOfWords+5)) {
		/* code */

		words->size += 10;
		/*WARNING: DONT REALLOC DIRECTLY TO SAME PARAM!-----------------------------------------*/
		if ((words->array = realloc(words->array, sizeof(Word)*words->size))==NULL) {
			/* code */
			printf(">    ERROR NOT ENOGH SPACE!!!\n");
			return 0;
		}
		if ((words->lines = realloc(words->lines, sizeof(int)*words->size))==NULL) {
			/* code */
			printf(">    ERROR NOT ENOGH SPACE!!!\n");
			return 0;
		}
		printf(">   ARRAY RESIZED!\n");
		return 1;
	}
	return 1;

}
void printInstructionsArray(Words *words){
	int i = 0;
	printf("> PRINTING WORDS ARRAY! ---------------------------------------------\n");

	while (i<words->numberOfWords) {
		Word currentWord;
		int currentLine;
		currentWord = words->array[i];
		currentLine = words->lines[i];
		switch (currentWord.command.grp) {
			case NOP:
				printf("COMMAND (%d) > LINE: %d GROUP: %d, OPCODE: %d, ~SRCAR~: %d, ~DESTAR~: %d\n",i,currentLine,currentWord.command.grp, currentWord.command.opcode, currentWord.command.srcar, currentWord.command.destar);
				i++;
				break;
			case ONEOP:
				printf("COMMAND (%d) > LINE: %d GROUP: %d, OPCODE: %d, ~SRCAR~: %d, DESTAR: %d\n",i,currentLine,currentWord.command.grp, currentWord.command.opcode, currentWord.command.srcar, currentWord.command.destar);
				i++;
				if(currentWord.command.destar == DIRECT_REGISTER_ADDRESS_RESOLUTION){
					printf("PARAMETERS (%d) > ~SRC~: %d DESTINATION: %d\n",i, words->array[i].paddress.src,words->array[i].paddress.dest);
				}else{
					printf("PARAMETERS (%d) > VALUE: %d\n",i, words->array[i].pvalue.value);
				}

				i++;
				break;
			case TWOP:
				printf("COMMAND (%d) > LINE: %d GROUP: %d, OPCODE: %d, SRCAR: %d, DESTAR: %d\n",i,currentLine,currentWord.command.grp, currentWord.command.opcode, currentWord.command.srcar, currentWord.command.destar);
				i++;
				if ((currentWord.command.srcar == DIRECT_REGISTER_ADDRESS_RESOLUTION)&&(currentWord.command.destar == DIRECT_REGISTER_ADDRESS_RESOLUTION)) {
					printf("PARAMETERS (%d) > SOURCE: %d, DESTINATION: %d \n",i,words->array[i].paddress.src, words->array[i].paddress.dest);
					i++;
				}else if(currentWord.command.srcar == DIRECT_REGISTER_ADDRESS_RESOLUTION){
					printf("PARAMETERS (%d - %d) > SOURCE: %d, ",i,i+1,words->array[i].paddress.src);
					i++;
					printf("DESTINATION: %d\n",words->array[i].pvalue.value );
					i++;
				}else if(currentWord.command.destar == DIRECT_REGISTER_ADDRESS_RESOLUTION){
					printf("PARAMETERS (%d - %d) > SOURCE: %d, ",i,i+1,words->array[i].pvalue.value);
					i++;
					printf("DESTINATION: %d\n",words->array[i].paddress.dest );
					i++;
				}else{
					printf("PARAMETERS (%d - %d) > SOURCE: %d, ",i,i+1,words->array[i].pvalue.value);
					i++;
					printf("DESTINATION: %d\n",words->array[i].pvalue.value );
					i++;

				}
				break;
			default:
				break;
		}
	}




	printf("------------------------------------------------------------------\n");

}
