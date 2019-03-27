CFLAGS = -Wall -pedantic -g
main: main.o
	gcc -o main main.o $(CFLAGS)
main.o: main.c
	gcc -c main.c $(CFLAGS)
clean:
	rm main.o main