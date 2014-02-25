LEVELDB_DIR = leveldb-1.15.0
CC = g++ 
OPTS = -I $(LEVELDB_DIR)/include -lpthread $(LEVELDB_DIR)/libleveldb.a
CCFLAGS = -O3 -Wall -g -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 # -pedantic

all: library.o msort bsort

library.o: library.cc library.h
	$(CC) -o $@ $(CCFLAGS)  -c $<

msort: msort.cc library.o
	$(CC) -o $@ $(CCFLAGS)  -c $<

bsort: bsort.cc
	$(CC) -o bsort bsort.cc $(OPTS)

clean:
	rm -rf *.o leveldb_dir

test: test.cc
	$(CC) -o test test.cc