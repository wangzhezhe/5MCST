CC=gcc
CFLAGS=-Wall -v

#inexplicit way
main: main.o hellofunc.o

#the key is the target name
#the value is the name of other target, or the file
#that the current target depends on
#main: main.o hellofunc.o hellofunc.h
#	gcc main.o hellofunc.o -o main

#main.o: main.c
#	gcc -c main.c -o main.o 

#-c Only run preprocess, compile, and assemble steps
#hellofunc.o: hellofunc.c
#	gcc -c hellofunc.c -o hellofunc.o 

clean:
	rm -f main main.o hellofunc.o



