CC = g++
INCLUDE = `xapian-config --cxxflags`
LIBS = `xapian-config --libs`
CCFLAGS = -O3 -Wall -g -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 # -pedantic

all: library.o indexer search 
test_indexer: test_indexer.cc
	$(CC) -o $@ $< $(LIBS) $(INCLUDE)
indexer: indexer.cc library.o
	$(CC) -o $@ $^ $(LIBS) $(INCLUDE)
search: search.cc library.o
	$(CC) -o $@ $^ $(LIBS) $(INCLUDE)
library.o: library.cc library.h
	$(CC) -o $@ -c $<
clean:
	rm -rf *.o test_indexer search indexer ... index library.o
