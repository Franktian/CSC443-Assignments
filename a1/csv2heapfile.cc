#include "library.h"
#include <stdio.h>
#include <iostream>

using namespace std;

void generate_random_csv_file(int record_size, int num_record, char* path_to_csv_file) {

}

void csv2heapfile(char* path_to_csv_file, char* path_to_heapfile, int page_size) {
    
    FILE* csv = fopen(path_to_csv_file, "r");
    if (!csv) {
    	cout << "ERROR csv2heapfile(): Input CSV file does not exist!" << endl;
        return;
    }
    FILE* output = fopen(path_to_heapfile, "w");
    Heapfile* heapfile = new Heapfile();
    // init_heapfile(heapfile, page_size, output);

    // for each line in cdv file 
    // {
    //     for each record r1 in the line

    //          page1 = find_free_page_on_heapfile (minSize)
    //          if page1 is not found
    //              page1 = alloc_page(heapfile)
    //          read_page(heap file, page1, page)
    //          add_fixed_len_page(page, r1) — 
    //          slot = find the slot
    //           — write_fixed_len_page(page, slot, r1) — call serialization function to make the record a series of bytes.
    //          write_page(page, heap file, page1)
             
    // }
}

int main(int argc, char* argv[]) {

	return 0;
}