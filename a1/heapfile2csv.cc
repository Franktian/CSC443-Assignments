#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>
#include <sys/timeb.h>


#include "library.h"

#define RECORD_SIZE 1000
#define HEAP_CSV_DEBUG 1

using namespace std;

int main( int argc, const char* argv[] )
{
    // Process the input parameters.
    if (argc != 4) {
        cout << "ERROR: invalid input parameters!" << endl;
        cout << "Please put: <csv_file> <heapfile> <page_size>" << endl;
        exit(1);
    }

    const char* csv_file = argv[1];
    const char* heapfile = argv[2];
    int page_size =  atoi((argv[3]));

    //Open csv_file to be written.
    FILE *csvFile = fopen(csv_file, "w");
    if (!csvFile) {
        cout << "ERROR: failed to open the csv file!" << endl;
	exit(1);
    }

    //Open heapfile to be read.
    FILE *heapFile = fopen(heapfile, "r+");
    if (!heapFile) {
        cout << "ERROR: failed to open the heapfile!" << endl;
	exit(1);
    }

    Heapfile *pHeapfile = new Heapfile;
    if (!pHeapfile) {
        cout << "ERROR: heap allocation of the heapfile failed!" << endl;
	exit(1);
    }
    pHeapfile->file_ptr = heapFile;
    pHeapfile->page_size = page_size;

    // Start the timer
    cout << "Start the timer" << endl;
    struct timeb _t;
    ftime(&_t);
    long start = _t.time * 1000 + _t.millitm;

    int pageID = 0;
    int numRecords = 0;
    //int firstRecord = 1;

    while(true) {

        // Initialize the record vector - 100 attributes of empty strings.
        // Otherwise, it causes weird errors.
        Record record;
        for(int i = 0; i < SCHEMA_ATTRIBUTES_NUM; i++){
            char content[10] = "         "; 
            record.push_back(content);
        }

        // Initialize a page for above records
        Page* page = new Page;
	if (!page) {
	    cout << "ERROR: dynamic allocation of the page failed!" << endl;
	    exit(1);
        }
        init_fixed_len_page(page, page_size, RECORD_SIZE);
        read_page(pHeapfile, pageID, page);
        
        // Empty page - means the processing is finished and get out of the loop.
        if (!*(char *)page->data) 
	    break;

	// Process the page and insert the records into CSV file.
        for (int slot = 0; slot < page->capacity; slot++) {

            // Read the page into records
            bool success = read_fixed_len_page(page, slot, &record, SCHEMA_ATTRIBUTE_LEN,
                SCHEMA_ATTRIBUTES_NUM);
            // Read an empty page? 
	    if (!success) {
		if (HEAP_CSV_DEBUG) 
		    cout << "The slot of the page is empty!" << endl;
		continue; 
	    }

            numRecords++;
            // Write the record to the CSV file.
            //if (firstRecord) {
            //    firstRecord = 0;
            //} 
	    //else {
            //    fprintf(csvFile, "\n");
            //}
           
            // Write the record to the CSV file.
            for (int i = 0; i < record.size(); i++) {
                // Append the new line character to the last attribute.
                if(i == record.size() - 1) {
                    fprintf(csvFile, "%s", record.at(i));
                } 
		else {
                    fprintf(csvFile, "%s,\n", record.at(i));
		    //fprintf(csvFile, "\n");
                }
            }
        }	
        pageID++;
    }

    // Finished converting, close the files.
    fclose(csvFile);
    fclose(heapFile);

    // Stop the timer
    cout << "Stop the timer" << endl;
    ftime(&_t);
    long finish = _t.time * 1000 + _t.millitm;
    long _time = finish - start;

    cout << "NUMBER OF RECORDS: " << numRecords << "\n";
    cout << "NUMBER OF PAGES: " << pageID << "\n";
    cout << "TIME : " << _time << " milliseconds\n";

    return 0;
}
