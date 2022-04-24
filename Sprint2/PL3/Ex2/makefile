writer: writer.o
		gcc -Wall -g writer.o -o writer -lrt

reader: reader.o
		gcc -Wall -g reader.o -o reader -lrt

writer.o: writer.c
		gcc -g -Wall -c writer.c -o writer.o -lrt

reader.o: reader.c
		gcc -g -Wall -c reader.c -o reader.o -lrt

clean:
		rm -f *.o writer reader

run: writer reader
		./writer
		./reader
