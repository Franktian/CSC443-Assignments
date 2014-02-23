LEVELDB_DIR = leveldb-1.15.0
CC = g++ 
OPTS = -I $(LEVELDB_DIR)/include -lpthread $(LEVELDB_DIR)/libleveldb.a
CCFLAGS = -O3 -Wall -g -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 # -pedantic

all: read write library.o

library.o: library.cc library.h
	$(CC) -o $@ $(CCFLAGS)  -c $<

read: read.cc
	$(CC) -o read read.cc $(OPTS)

write: write.cc
	$(CC) -o write write.cc $(OPTS)

clean:
	rm -rf read write *.o leveldb_dir

test: all
	./write
	./read
