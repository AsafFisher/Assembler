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
        char *output = NULL;
        char *externout = NULL;
        char *entryout = NULL;

        if(!(fi = (openFile(argv[i], ".as", "r")))){
			fprintf(stderr,"-------------------------FATAL-------------------------\n");
			continue;
		}

		if(!firstEntry(fi)){
			continue;
		}
        printArr();
        printSymbols();
		rewind(fi);
        externFile();
        if(!secondEntry(fi)){
            continue;
        }
        fclose(fi);


        if((entryout = getEntrys())!=NULL) {
            if(!(fi = openFile(argv[i],".ent","w"))){
                fprintf(stderr,"-------------------------FATAL-------------------------\n");
                continue;
            }
            fprintf(fi, "%s", entryout);
            fclose(fi);
            free(entryout);
        }


        if((externout = getExterns())!=NULL) {
            if(!(fi = openFile(argv[i],".ext","w"))){
                fprintf(stderr,"-------------------------FATAL-------------------------\n");
                continue;
            }
            fprintf(fi, "%s", externout);
            fclose(fi);
            free(externout);
        }

        if(!(fi = openFile(argv[i],".ob","w"))){
            fprintf(stderr,"-------------------------FATAL-------------------------\n");
            continue;
        }
        printArr();
        output = convertToBase8();
        fprintf(fi,"%s",output);
        fclose(fi);
        free(output);
        freeAll();



	}
	return 0;

}


int secondEntry(FILE* file){
    if(!variableLinker(file)){
        printf("Assembler > ERROR Assembler throw exception!");
        freeAll();
        return 0;
    }
    return 1;
}
int firstEntry(FILE* input){
	int lineNumber = 1;
    int ERRORS = 0;

	char line[LINE_MAX];
	while(fgets(line,LINE_MAX,input)!=NULL){
		if(!parseLine(line,lineNumber,stderr)){
			printf("Assembler > ERROR Assembler throw exception! (LINE: %d) \nabort file!\n",lineNumber);
			ERRORS++;
		}
		lineNumber++;

	}
    if(ERRORS>0){
        printf("FOUND '%d' ERRORS in code.",ERRORS);
        freeAll();
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






