#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <sys/timeb.h>

#include "library.h"

using namespace std;

/** 
 * csv2heapfile function. 
 */ 

int main( int argc, const char* argv[] )
{
    // Process the input parameters.
    if (argc != 4) {
	cout << "ERROR: invalid input parameters!" << endl;
        cout << "Please put: <csv_file> <heapfile> <page_size>" << endl;
        exit(1);
    }

    const char* csvFileName = argv[1];
    char* heapfileName = (char*)argv[2];
    int page_size =  atoi((argv[3]));

    Heapfile *hFile = new Heapfile();
    if (!hFile) {
   	cout << "ERROR: dynamic allocation for the Heapfile object failed!" << endl;
	exit(1);
    }

    // Truncate file to zero length or create a file for writing
    // READ & WRITE
    FILE *pFile = fopen(heapfileName, "w+");

    init_heapfile(hFile, page_size, pFile);

    Page* page = new Page();

    // Initialize page
    init_fixed_len_page(page, page_size, RECORD_SIZE);

    // Open the csv file
    ifstream data; // csv file handler
    data.open(csvFileName);
    string line; // contains the record read from csv file.

    // Record Start Time 
    cout << "Start the timer" << endl;
    struct timeb _t;                
    ftime(&_t);
    long start = _t.time * 1000 + _t.millitm;        

    int index = 0;        
    int numRecs = 0;        
    int numPages = 0;
    while(getline(data,line)) {

        stringstream lineStream(line); // lineStream is used to process the record read from the csv file
        string dataField; // contains the attribute of the record.

        // Initialize a record vector - create 100 attributes of empty strings.
        Record record;
        for(int i = 0; i < SCHEMA_ATTRIBUTES_NUM; i++) {
            char content[10] = "         ";
            record.push_back(content);
        }   

        // Process the record in the csv file.
	    int slot = 0;
        while(getline(lineStream,dataField,',')) {
            char* attribute = new char[strlen(dataField.c_str())];
            strncpy(attribute, dataField.c_str(), strlen(dataField.c_str()));
 	        record.at(slot++) = attribute;
            //record.push_back(attribute);
        }

        // Write the record to the page
        write_fixed_len_page(page, index++, &record, SCHEMA_ATTRIBUTE_LEN);
        numRecs++;

        // Page is full, allocate a new page and write the current one back to heapfile
        if (index == page->capacity) {
            
            PageID id = alloc_page(hFile);
            cout << "allocated page ID: " << id << endl;
            write_page(page, hFile, id);
            
            // Initialize new page
            delete page;
            page = new Page();
            init_fixed_len_page(page, page_size, RECORD_SIZE);
            index = 0;
            numPages++;
        }
    }
  
    // Remaining page now written yet, write the last page to file
    if(index != 0) {
        PageID id = alloc_page(hFile);
        write_page(page, hFile, id);
        numPages++;
    }

    delete page;
    data.close();
    fclose(pFile);

    // Stop the timer
    cout << "Stop the timer" << endl;
    ftime(&_t);
    long finish = _t.time * 1000 + _t.millitm;
    long _time = finish - start;

    cout << "NUMBER OF RECORDS : " << numRecs << endl;
    cout << "NUMBER OF PAGES : " << numPages << endl;
    cout << "TIME : " << _time << " milliseconds" << endl;

    return 0;
}
