# Makefile

CC = g++ 
CCFLAGS = -O3 -Wall -g -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 # -pedantic

write_fixed_len_pages: write_fixed_len_pages.o library.o
	$(CC) $(CCFLAGS) write_fixed_len_pages.o library.o -o write_fixed_len_pages

read_fixed_len_pages: read_fixed_len_pages.o library.o
	$(CC) $(CCFLAGS) read_fixed_len_pages.o library.o -o read_fixed_len_pages

library.o: library.cc library.h
	$(CC) -o $@ $(CCFLAGS)  -c $<

write_fixed_len_pages.o: write_fixed_len_pages.cc library.h
	$(CC) $(CCFLAGS)  -c write_fixed_len_pages.cc

read_fixed_len_pages.o: read_fixed_len_pages.cc library.h
	$(CC) $(CCFLAGS) -c read_fixed_len_pages.cc

csv2heapfile: csv2heapfile.cc library.o
	$(CC) -o $@ $< library.o
 
heapfile2csv: heapfile2csv.cc library.o
	$(CC) -o $@ $< library.o
 
query: query.cc library.o
	$(CC) -o $@ $< library.o

clean:
	rm -rf *.o
	rm -rf write_fixed_len_pages read_fixed_len_pages
