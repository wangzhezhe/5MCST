CC=gcc
CFLAGS=-Wall -fPIC
# if not add ld flags, there is issue for finding .so library
LDFLAGS=-Wl,-rpath=./
main: main.o hellofunc.so
	gcc $(CFLAGS) $(LDFLAGS) -o main main.o hellofunc.so

hellofunc.so: 
	gcc $(CFLAGS) -shared -o hellofunc.so -fPIC hellofunc.c

clean:
	rm -f main main.o hellofunc.o hellofunc hellofunc.so
