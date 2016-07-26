#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"

#define LINE_MAX 80
int firstEntry(FILE* input);
int main(int argc, const char * argv[]){
	int i;
	FILE *fi;

	for(i=1;i<argc;i++){
		if(!(fi = fopen(argv[i], "r"))){
			fprintf(stderr,"-------------------------FATAL-------------------------\n");
			continue;
		}

		if(!firstEntry(fi)){
			continue;
		}
		rewind(fi);

	}
	return 0;

}

int firstEntry(FILE* input){
	int lineNumber = 1;


	char line[LINE_MAX];
	while(fgets(line,LINE_MAX,input)!=NULL){
		if(!parseLine(line,lineNumber)){
			printf("Assembler > ERROR Assembler throw exception! (LINE: %d) \nabort file!\n",lineNumber);
			break;
		}
		lineNumber++;




	}
	printArr();
	return 1;
}
