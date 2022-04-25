#makefile
ex14: main.o
		gcc main.o -o ex15 -lrt

main.o: main.c
		gcc -g -Wall -c main.c -o main.o -lrt

clean:
		rm -f *.o ex15

run: ex14
		./ex14
