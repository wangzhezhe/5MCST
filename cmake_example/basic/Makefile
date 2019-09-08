
all: main

sqrt.o: ./util/sqrt.cpp
	g++ -c ./util/sqrt.cpp
main : main.cpp sqrt.o
	g++ -o main main.cpp sqrt.o

clean :
	rm main *.o