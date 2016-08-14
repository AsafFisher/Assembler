#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"

FILE *openFile(const char *name,const char *ending ,const char *mode);
int firstEntry(FILE* input);
int secondEntry(FILE* input);
int main(int argc, const char * argv[]){
	int i;
	FILE *fi;

	for(i=1;i<argc;i++){
		if(!(fi = (openFile(argv[i], ".as", "r")))){
			fprintf(stderr,"-------------------------FATAL-------------------------\n");
			continue;
		}

		if(!firstEntry(fi)){
			continue;
		}
        printArr();
        printUndefineds();
        printSymbols();
		rewind(fi);
        if(!secondEntry(fi)){
            continue;
        }
        printArr();
        convertToBase8();

	}
	return 0;

}


int secondEntry(FILE* file){
    if(!variableLinker(file)){
        printf("Assembler > ERROR Assembler throw exception!");
        return 0;
    }
    return 1;
}
int firstEntry(FILE* input){
	int lineNumber = 1;
    int ERRORS = 0;

	char line[LINE_MAX];
	while(fgets(line,LINE_MAX,input)!=NULL){
		if(!parseLine(line,lineNumber)){
			printf("Assembler > ERROR Assembler throw exception! (LINE: %d) \nabort file!\n",lineNumber);
			ERRORS++;
		}
		lineNumber++;

	}
    if(ERRORS>0){
        printf("FOUND '%d' ERRORS in code.",ERRORS);
        return 0;
    }
	return 1;
}

FILE *openFile(const char *name,const char *ending,const char *mode)
{
	FILE *file;
	char name_as[MAX_FILE_NAME];
	strcpy(name_as,name);
	strcat(name_as,ending);
	file = fopen(name_as, mode);
	return file;
}






