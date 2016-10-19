## Student Name : Tessa(Hyeri) Song
## Login id : songt
## Sat 16 Apr 2016
##
## Makefile 
##

## CC  = Compiler.
## CFLAGS = Compiler flags.
CC	= gcc
CFLAGS 	= -Wall  


## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.

SRC =		simulation.c queue.c memory_manage.c
OBJ =		memory_manage.o queue.o simulation.o
EXE = 		./simulation


## Top level target is executable.
$(EXE):	$(OBJ)
		$(CC) $(CFLAGS) -o $(EXE) $(OBJ) -lm



## Clean: Remove object files and core dump files.
clean:
		/bin/rm $(OBJ) 


## Clobber: Performs Clean and removes executable file.

clobber: clean
		/bin/rm $(EXE) 


## Dependencies

simulation.o:	queue.h memory_manage.h Makefile
queue.o:	queue.h
memory_manage.o: memory_manage.h
