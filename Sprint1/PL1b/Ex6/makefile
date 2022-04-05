main: a.o 
	gcc a.o -o a
main.o: a.c 
	gcc -Wall -g -c a.c -o a.o
run: a
	./a
	
b: b.o 
	gcc b.o -o b
b.o: b.c 
	gcc -Wall -g -c b.c -o b.o
runb: b
	./b
clean:s
	rm ∗ . o a
