all: writer reader

writer: writer.c
	gcc -lrt -lm -Wall -g -o writer writer.c

reader: reader.c
	gcc -lrt -lm -Wall -g -o reader reader.c

clean:
	rm -f writer reader

run_reader: reader
	./reader

run_writer: writer
	./writer
