CC = g++
XAPIAN_DIR = xapian-core-1.2.13
INCLUDE = -I $(XAPIAN_DIR)/include -L $(XAPIAN_DIR)/.lib
LIBS = -lxapian -lz -luuid
CCFLAGS = -O3 -Wall -g -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 # -pedantic

test: test.cc
	$(CC) -o $@ $< $(LIBS) $(INCLUDE)

clean:
	rm -rf *.o test