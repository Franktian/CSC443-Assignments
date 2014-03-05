CC = g++
INCLUDE = `xapian-config --cxxflags`
LIBS = `xapian-config --libs`
CCFLAGS = -O3 -Wall -g -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 # -pedantic

test: test.cc
	$(CC) -o $@ $< $(LIBS) $(INCLUDE)

bio_indexer: bio_indexer.cc
	$(CC) -o $@ $< $(LIBS) $(INCLUDE)

bio_search: bio_search.cc
	$(CC) -o $@ $< $(LIBS) $(INCLUDE)

word_indexer: word_index.cc
	$(CC) -o $@ $< $(LIBS) $(INCLUDE)

clean:
	rm -rf *.o test bio_search word_indexer bio_indexer ...