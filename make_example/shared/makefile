CC=gcc
CFLAGS=-Wall -fPIC

main: main.o hellofunc.so

hellofunc.so: 
	gcc $(CFLAGS) -shared -o hellofunc.so -fPIC hellofunc.c

clean:
	rm -f main main.o hellofunc.o hellofunc hellofunc.so