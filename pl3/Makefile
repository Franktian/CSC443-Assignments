CC = g++
INCLUDE = `xapian-config --cxxflags`
LIBS = `xapian-config --libs`
CCFLAGS = -O3 -Wall -g -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 # -pedantic

test: test.cc
	$(CC) -o $@ $< $(LIBS) $(INCLUDE)

clean:
	rm -rf *.o test ...