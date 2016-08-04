#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"

int firstEntry(FILE* input);
int secondEntry(FILE* input);
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
        if(!secondEntry(fi)){
            continue;
        }

	}
	return 0;

}

int secondEntry(FILE* file){
    variableLinker(file);
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
    printUndefineds();
    printSymbols();
	return 1;
}
