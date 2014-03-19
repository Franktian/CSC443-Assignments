CC = g++
INCLUDE = `xapian-config --cxxflags`
LIBS = `xapian-config --libs`
CCFLAGS = -O3 -Wall -g -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 # -pedantic

test_indexer: test_indexer.cc
	$(CC) -o $@ $< $(LIBS) $(INCLUDE)

bio_indexer: bio_indexer.cc
	$(CC) -o $@ $< $(LIBS) $(INCLUDE)

bio_search: bio_search.cc
	$(CC) -o $@ $< $(LIBS) $(INCLUDE)
library: library.cc
	$(CC) -o $@ $< $(LIBS) $(INCLUDE)
clean:
	rm -rf *.o test_indexer bio_search bio_indexer ... indexer library
