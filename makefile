runfile:	assembler.o protocols.o
	gcc	-g	-ansi	-Wall	assembler.o	protocols.o	-o	runfile
assembler.o:	assembler.c
	gcc	-c	-ansi	-Wall	-pedantic	assembler.c -o assembler.o
protocols.o:	protocols.c
	gcc	-c	-ansi	-Wall	-pedantic	protocols.c -o protocols.o

