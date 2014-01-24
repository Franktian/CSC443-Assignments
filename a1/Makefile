# Makefile

CC = g++

test:
	make clean
	make library.o
	make test.o
	./testbin/test

test.o: test.cc library.h
	$(CC) -o $@ -c $<
	$(CC) test.o library.o -o ./testbin/test 

clean:
	rm -rf *.o

library.o: library.cc library.h
	$(CC) -o $@ -c $<

csv2heapfile: csv2heapfile.cc library.o
	$(CC) -o $@ $< library.o
 
heapfile2csv: heapfile2csv.cc library.o
	$(CC) -o $@ $< library.o
 
query: query.cc library.o
	$(CC) -o $@ $< library.o