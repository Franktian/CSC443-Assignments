#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <sys/timeb.h>

#include "library.h"
using namespace std;

// General Structure of the Schema
// Record Size: 100 attributes * 10 bytes = 1000
#define RECORD_SIZE 1000
// Number of columns = number of attributes = 100
#define NUM_COLUMNS 100
// Debug code enabler
#define DEBUG 0

// Buffer needs to have enough space for the records 
// Namely, 100 attributes with 10 bytes each, the 99 commas, the newline, and a \0.
#define BUFFER_SIZE (RECORD_SIZE + NUM_COLUMNS + 1)

int main(int argc, char **argv) {

    // Extract the inputs
    if (argc != 4) {
        cout << "ERROR: Invalid input paramenters. Please follow the pattern." << endl;
        printf("%s <csv file> <page file> <page size>\n", argv[0]);
        exit(1);
    }   

    size_t page_size = atoi(argv[3]);

    ifstream csvf(argv[1]);
    FILE *pageFile = fopen(argv[2], "w");
    if (!csvf || !pageFile) {
        printf("Error opening file(s).\n");
        exit(1);
    }   

    // Start the timer
    cout << "Start the timer" << endl;
    struct timeb _t;                
    ftime(&_t);
    long init = _t.time * 1000 + _t.millitm;


    Page page;
    cout << "Initializing the page..." << endl;
    init_fixed_len_page(&page, page_size, RECORD_SIZE);

    int total_records = 0;
    int page_count = 0;

    int capacity = fixed_len_page_capacity(&page);
    int added = 0; // number of records added to the current page
    cout << "Process the CSV file..." << endl;
    while (csvf) {
        Record r;
	if (DEBUG) cout << "Loop entered..." << endl;

        string buf;
        buf.reserve(RECORD_SIZE);

	if (DEBUG) cout << "Get the record from CSV file" << endl;
        if (!getline(csvf, buf))
            break;

        // Fill the record with the CSV data
	if (DEBUG) cout << "Fill the record with the CSV data" << endl;
        istringstream stream(buf);

        // Not using strtok anymore. Major bug:
        //      strtok is repeating the first column of the last row after
        //      returning the last column of the last row. I have no idea why.
        //
        // Yeah... that was fun to track down.
        int pos = 0;
        while (stream) {

	    if (DEBUG) cout << "Parsing the record..." << endl;
            char attr[11];
            if (!stream.getline(attr, 11, ',')) break;
	    r.push_back(attr);
        }

	if (DEBUG) cout << "Adding the record to the page..." << endl;
        int slot = add_fixed_len_page(&page, &r);
        added++;
        assert(slot != -1);

        if (added == capacity) {
            // Write page to file and re-initialize.
            fwrite(page.data, 1, page.page_size, pageFile);
            init_fixed_len_page(&page, page_size, RECORD_SIZE);
            page_count++;
            total_records += added;
            added = 0;
        }
    }

    // Unfilled pgae
    if (added > 0) {
        fwrite(page.data, 1, (1 + RECORD_SIZE) * added, pageFile);
        page_count++;
        total_records += added;
    }

    ftime(&_t);
    long done = _t.time * 1000 + _t.millitm;
    long time = done-init;

    printf("NUMBER OF RECORDS: %d\n", total_records);
    printf("NUMBER OF PAGES: %d\n", page_count);
    printf("TIME: %ld milliseconds\n", time);

    fclose(pageFile);
    csvf.close();
    return 0;
}

