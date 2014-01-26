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
#define NUM_ATTRIBUTES 100

using namespace std;

//void write_page_to_file(char* fname, Page* page){
//    FILE * f;
//    f = fopen(fname, "a");
//    char* buf = (char *)page->data;
//    fwrite (buf, 1 , page->page_size , f);
//    fclose (f);
//}

int main( int argc, const char* argv[] )
{

    if (argc != 4) {
 	cout << "ERROR: invalid input parameters!" << endl; 
        cout << "Please put: <csv_file> <page_file> <page_size>" << endl;
        return 0;
    }

    const char* csvFile = argv[1];
    char* pageFile = (char*)argv[2];
    int pageSize =  atoi((argv[3]));

        
    //Page* page = (Page *)malloc(sizeof(Page));
    Page *page = new Page();
    if (!page) {
	cout << "ERROR: something wrong with memory, page heap allocation failed!" << endl;
	exit(1);
    }

    // Initialize page
    init_fixed_len_page(page, pageSize, RECORD_SIZE);
        
    // Open the csv file
    ifstream data(csvFile);
    string line;

    //Record Start Time 
    cout << "Start the timer" << endl;
    struct timeb _t;                
    ftime(&_t);
    long init = _t.time * 1000 + _t.millitm;
     
    int j = 0;                
    int numPages = 0;
    int numRecords = 0;
    while(getline(data,line)) {

    	//Initialize a record vector - create 100 attributes of empty strings.
        Record record;
       	for(int i = 0; i < NUM_ATTRIBUTES; i++){
            V content = "          ";
            record.push_back(content);
        }

        stringstream lineStream(line);
        string cell;

        int field = 0;

        while(getline(lineStream,cell,',')) {
            char* attribute = new char[strlen(cell.c_str())];
            strcpy(attribute, cell.c_str());
            record.at(field) = attribute;
            field++;
        } 

        // Write the record to page
        write_fixed_len_page(page, j++, &record);
        numRecords++;

        // If page is full, write the current page to file
	// create a new page. 
        if (j == page->capacity){

	    FILE * f = fopen(pageFile, "a");
	    if (!f) {
		cout << "ERROR: cannot open the page file!" << endl;
		exit(1);
	    }

            fwrite ((char*)page->data, 1 , page->page_size , f); 
            fclose (f);

            // write_page_to_file(pageFile, page);
            // Initialize a new page to store the records
            init_fixed_len_page(page, pageSize, RECORD_SIZE);
            j = 0;
            numPages++;
        }
    }

    // process the remaining records
    if(j != 0) {

        FILE * f = fopen(pageFile, "a");
        if (!f) {
            cout << "ERROR: cannot open the page file!" << endl;
            exit(1);
        }   
        fwrite ((char*)page->data, 1 , page->page_size , f); 
        fclose (f);
        numPages++;
    }
        
    data.close();
    delete page;

    // Stop the timer
    cout << "Stop the timer" << endl;
    ftime(&_t);
    long done = _t.time * 1000 + _t.millitm;
    long _time = done-init;

    cout << "NUMBER OF RECORDS : " << numRecords << "\n";
    cout << "NUMBER OF PAGES : " << numPages << "\n";
    cout << "TIME : " << _time << " milliseconds\n";

    return 0;
}
