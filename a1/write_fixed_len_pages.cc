#include <sys/timeb.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "library.h"

#define RECORD_SIZE 1000

using namespace std;

int main( int argc, const char* argv[] )
{

    if (argc != 4) {
 	cout << "ERROR: invalid input parameters!" << endl; 
        cout << "Please put: <csv_file> <page_file> <page_size>" << endl;
        return 0;
    }

    const char* csvFile = argv[1];
    char* pageFileName = (char*)argv[2];
    FILE *page_file = fopen(pageFileName, "w");
    if (!page_file) {
    	cout << "ERROR: failed to open page file!" << endl;
	exit(1);
    } 

    int pageSize = atoi((argv[3]));
        
    Page *page = new Page();
    if (!page) {
	cout << "ERROR: something wrong with memory, page heap allocation failed!" << endl;
	exit(1);
    }

    // Initialize page
    init_fixed_len_page(page, pageSize, RECORD_SIZE);
        
    ifstream data; // csv file stream.
    string line; // Holds the records from the csv file.

    // Start the timer
    cout << "Start the timer" << endl;
    struct timeb _t;                
    ftime(&_t);
    long start = _t.time * 1000 + _t.millitm;
     
    int index = 0;                
    int numPages = 0;
    int numRecords = 0;
    // Open the csv file.
    data.open(csvFile);

    // Loop - iterate over all the records in the csv file and extract the records.
    while(getline(data,line)) {

    	// Initialize a record vector - create 100 attributes of empty strings.
        Record record;
       	for(int i = 0; i < SCHEMA_ATTRIBUTES_NUM; i++) {
            char content[10] = "         ";
            record.push_back(content);
        }

        stringstream lineStream(line);
        string dataField;

	// Process the record read from the pageFile
        int slot = 0;
        while(getline(lineStream,dataField,',')) {
            char* attribute = new char[strlen(dataField.c_str())];
            strncpy(attribute, dataField.c_str(), strlen(dataField.c_str()));
            record.at(slot) = attribute;
            slot++;
        } 

        // Write the record to page
        write_fixed_len_page(page, index++, &record, SCHEMA_ATTRIBUTE_LEN);
        numRecords++;

        // If page is full, write the current page to file create a new page. 
        if (index == page->capacity){
            fwrite ((char*)page->data, 1 , page->page_size , page_file); 

            // Initialize a new page to store the records
	    delete (char*)page->data; // Prevent memory leak
            init_fixed_len_page(page, pageSize, RECORD_SIZE);
            index = 0;
            numPages++;
        }
    }

    // Process the remaining records - write the page that is not full to page file.
    if(index != 0) {
        fwrite ((char*)page->data, 1 , page->page_size , page_file); 
        numPages++;
    }
        
    fclose(page_file);
    data.close();
    delete page;

    // Stop the timer
    cout << "Stop the timer" << endl;
    ftime(&_t);
    long finish = _t.time * 1000 + _t.millitm;
    long _time = finish - start;

    cout << "NUMBER OF RECORDS : " << numRecords << "\n";
    cout << "NUMBER OF PAGES : " << numPages << "\n";
    cout << "TIME : " << _time << " milliseconds\n";

    return 0;
}
