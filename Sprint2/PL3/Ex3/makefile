all: pipe sharedmemory

pipe: pipe.c
	gcc -lrt -lm -Wall -g -o pipe pipe.c

sharedmemory: sharedmemory.c
	gcc -lrt -lm -Wall -g -o sharedmemory sharedmemory.c

clean:
	rm -f pipe sharedmemory

run: pipe sharedmemory
	./pipe
	./sharedmemory
