LEVELDB_DIR = leveldb-1.15.0
CC = g++ 
OPTS = -I $(LEVELDB_DIR)/include -lpthread $(LEVELDB_DIR)/libleveldb.a

all: read write
	
read: read.cc
	$(CC) -o read read.cc $(OPTS)

write: write.cc
	$(CC) -o write write.cc $(OPTS)

clean:
	rm -rf read write leveldb_dir

test: all
	./write
	./read
