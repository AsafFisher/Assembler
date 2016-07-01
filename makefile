runfile:	assembler.o protocols.o lineparser.o
	gcc	-g	-ansi	-Wall	assembler.o	protocols.o	lineparser.o	-o	runfile
assembler.o:	assembler.c
	gcc	-c	-ansi	-Wall	-pedantic	assembler.c -o assembler.o
protocols.o:	protocols.c
	gcc	-c	-ansi	-Wall	-pedantic	protocols.c -o protocols.o	-lm
lineparser.o:	lineparser.c
	gcc	-c	-ansi	-Wall	-pedantic	lineparser.c -o lineparser.o
