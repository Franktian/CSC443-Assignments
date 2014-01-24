# Makefile

CC = g++


read_fixed_len_pages: read_fixed_len_pages.o library.o
	$(CC) read_fixed_len_pages.o library.o -o read_fixed_len_pages

write_fixed_len_pages: write_fixed_len_pages.o library.o
	$(CC) write_fixed_len_pages.o library.o -o write_fixed_len_pages

write_fixed_len_pages.o: write_fixed_len_pages.cc library.h
	$(CC) -c write_fixed_len_pages.cc

read_fixed_len_pages.o: read_fixed_len_pages.cc library.h
	$(CC) -c read_fixed_len_pages.cc

library.o: library.cc library.h
	$(CC) -o $@ -c $<

csv2heapfile: csv2heapfile.cc library.o
	$(CC) -o $@ $< library.o
 
heapfile2csv: heapfile2csv.cc library.o
	$(CC) -o $@ $< library.o
 
query: query.cc library.o
	$(CC) -o $@ $< library.o

clean:
	rm -rf *.o
