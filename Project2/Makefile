## Student Name : Tessa(Hyeri) Song
## Login id : songt
## Fri 27 May 2016
##
## Makefile 
##

## CC  = Compiler.
## CFLAGS = Compiler flags.
CC	= gcc
CFLAGS 	= -Wall -lpthread -lnsl


## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.

SRC_S =		server.c
OBJ_S =		server.o
EXE_S = 	./server

SRC_C =		client.c 
OBJ_C =		client.o
EXE_C = 	./client

## Produce server program
server: $(OBJ_S)
		$(CC) $(CFLAGS) -o $(EXE_S) $(OBJ_S) -lm


## Produce client program
client: $(OBJ_C)
		$(CC) $(CFLAGS) -o $(EXE_C) $(OBJ_C) -lm


## Clean: Remove object files and core dump files.
clean:
		/bin/rm $(OBJ_C) $(OBJ_S) 



## Clobber: Performs Clean and removes executable file.
clobber: clean
		/bin/rm $(EXE_C) $(EXE_S) 


## Dependencies
client.o:	client.h   Makefile
server.o:	server.h   Makefile





