
#include <stdio.h>
#include <stdlib.h>
#include "protocols.h"
#include "lineparser.h"
#define LINE_MAX 80

int firstEntry(FILE* input);
int IC;
int DC;

int main(int argc, const char * argv[]){
	int i;
	FILE *fi;

	for(i=1;i<argc;i++){
		IC = 100;
		DC = 0;
		if(!(fi = fopen(argv[i], "r"))){
			fprintf(stderr,"ERROR:\n");
			continue;
		}

		if(!firstEntry(fi)){
			/*failed.*/
			continue;
		}
	}

	return 0;

}

int firstEntry(FILE* input){
	char line[LINE_MAX];
	while(fgets(line,LINE_MAX,input)!=NULL){
		parseLine(line);
		/*fprintf(stdout,"%s\n",line);*/



	}
	return 1;
}
