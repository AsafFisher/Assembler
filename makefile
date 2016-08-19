runfile:	assembler.o protocols.o lineparser.o legality.o
	gcc	-g	-ansi	-Wall	assembler.o	protocols.o	lineparser.o legality.o 	-o	runfile
assembler.o:	assembler.c
	gcc	-c	-ansi	-Wall	-pedantic	assembler.c -o assembler.o
protocols.o:	protocols.c
	gcc	-c	-ansi	-Wall	-pedantic	protocols.c -o protocols.o
lineparser.o:	lineparser.c
	gcc	-c	-ansi	-Wall	-pedantic	lineparser.c -o lineparser.o
legality.o:	legality.c
	gcc	-c	-ansi	-Wall	-pedantic	legality.c -o legality.o
