CC=gcc
CFLAGS=-Wall -v

main: main.o hellofunclib
	gcc $(CFLAGS) main.o -I ./mylib -L ./mylib -lhellofunc -o main

main.o: main.c
	gcc $(CFLAGS) -c main.c -I ./mylib -o main.o 

hellofunclib: ./mylib/hellofunc.c
	gcc $(CFLAGS) -c ./mylib/hellofunc.c -o ./mylib/hellofunc.o 
	ar rcs ./mylib/libhellofunc.a ./mylib/hellofunc.o

clean:
	rm -f main main.o ./mylib/*.a ./mylib/*.o 


