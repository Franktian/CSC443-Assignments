#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/timeb.h>

#include "library.h"

using namespace std;

// In this assignment, the record size is fixed - 1000 bytes
#define RECORD_SIZE 1000

int main(int argc, char **argv) {

    // Processing the input parameter
    if (argc != 3) {
	cout << "ERROR: invalid input parameters!" << endl;
        cout << "Please put: <page file> <page size>" << endl;
        exit(1);
    }   

    //size_t pageSize = atoi(argv[2]);
    int pageSize = atoi(argv[2]);

    FILE *csvf = stdout; // Out put the record to terminal
    FILE *pageFile = fopen(argv[1], "r");
    if (!csvf || !pageFile) {
        cout << "ERROR: failed to file(s)." << endl;
        exit(1);
    }   

    cout << "Start the timer" << endl;
    struct timeb _t;
    ftime(&_t);
    long init = _t.time * 1000 + _t.millitm;

    int pageCount = 0;
    int recordCount = 0;

    // Initial the page which stores the data read from the pageFile
    Page page; 
    init_fixed_len_page(&page, pageSize, RECORD_SIZE);

    // Load a page of data from the pageFile
    fread(page.data, 1, page.page_size, pageFile);

    while (!feof(pageFile)) {

        pageCount++;

        // For each page, iterate over all the slots and prints the records to terminal
        for (int i = 0; i < page.capacity; ++i) {

                Record r;
		// Initialize the record vector - 100 attributes of empty strings.
		// Otherwise, it causes weird errors.
                for(int i = 0; i < SCHEMA_ATTRIBUTES_NUM; i++){
                         V content = "          ";
                         r.push_back(content);
                 }   

		// For empty slots in the page, we do not increment the record count.
		// read_fixed_len_page() return false means the slot is empty.
                if (read_fixed_len_page(&page, i, &r)) {
		    // Parse the serialized data and print it to the terminal.
                    for(int i = 0; i < r.size(); i++){
			// Last attribute - do not append comma but a new line charater
                        if(i == r.size() - 1){
			    cout << (char*) r.at(i) << endl;
                        } 
		        else {
		            cout << (char*) r.at(i) << ",";
                        }
                    }
                    recordCount++;
 		}
        }

	char *position = (char*)page.data;
        fread(position, 1, page.page_size, pageFile);
    }

    // Stop the timer
    cout << "Stop the timer" << endl;
    ftime(&_t);
    long done = _t.time * 1000 + _t.millitm;
    long time = done-init;

    printf("NUMBER OF RECORDS: %d\n", recordCount);
    printf("NUMBER OF PAGES: %d\n", pageCount);
    printf("TIME: %ld milliseconds\n", time);

    fclose(pageFile);
    return 0;
}


