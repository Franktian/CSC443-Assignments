#include <cstdlib>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>
#include <sys/timeb.h>


#include "library.h"

#define RECORD_SIZE 1000
#define HEAP_CSV_DEBUG 0

using namespace std;

int main( int argc, const char* argv[] )
{
    // Process the input parameters.
    if (argc != 4) {
        cout << "ERROR: invalid input parameters!" << endl;
        cout << "Please put: <heapfile> <csv_file> <page_size>" << endl;
        exit(1);
    }

    const char* heapfile = argv[1];
    const char* csv_file = argv[2];
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

    int pageID;
    int numRecords = 0;
    int numPages = 0;
    //int firstRecord = 1;

    PageIterator* iterator = new PageIterator(pHeapfile);
    while(iterator->hasNext()) {

        // Get the next page ID
        pageID = iterator->next();

        // Initialize the record vector - 100 attributes of empty strings.
        // Otherwise, it causes weird errors.
        Record record;
        for(int i = 0; i < SCHEMA_ATTRIBUTES_NUM; i++){
            char content[10] = "         "; 
            record.push_back(content);
        }

        // Initialize a page for above records
        Page* page = new Page();
        
        // Read the page from the heapfile
        init_fixed_len_page(page, page_size, RECORD_SIZE);
        read_page(pHeapfile, pageID, page);

	    // Process the page and insert the records into CSV file.
        for (int slot = 0; slot < page->capacity; slot++) {
            // Read the page into records
            bool success = read_fixed_len_page(page, slot, &record, 
                SCHEMA_ATTRIBUTE_LEN, SCHEMA_ATTRIBUTES_NUM);
            
            if (!success) break;
            
            // Write the record to the CSV file.
            for (int i = 0; i < record.size(); i++) {
                // Append the new line character to the last attribute.
                if (i == record.size() - 1) {
                    fprintf(csvFile, "%s\n", record.at(i));
                } else {
                    fprintf(csvFile, "%s,", record.at(i));
                }
            }
            numRecords++;
        }
        numPages++;
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
    cout << "NUMBER OF PAGES: " << numPages << "\n";
    cout << "TIME : " << _time << " milliseconds\n";

    return 0;
}
