ex12: main.o
		gcc main.o -o main

main.o: main.c
		gcc -g -Wall -c main.c -o main.o

clean:
		rm -f *.o main

run: main
		./main
