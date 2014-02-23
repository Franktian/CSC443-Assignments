#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <cstring>
#include <sys/timeb.h>

#include "leveldb/db.h"

#define DEBUG_TERM 0;
#define DEBUG_FILE 0;

using namespace std;

/**
 * The input file is CSV file 
 */
int main(int argc, const char* argv[]) {

	if (argc != 3) {
		cout << "ERROR: invalid input parameters!" << endl;
		cout << "Please enter <input_file> <out_index>" << endl;
		exit(1);
	}

	char* inputFile = (char*)argv[1];
	char* outIndex = (char*)argv[2];
	int numRecs = 0;

 	// Opens a database connection to "out_index"
	leveldb::DB *db;
  	leveldb::Options options;
  	options.create_if_missing = true;
  	leveldb::Status status = leveldb::DB::Open(options, outIndex, &db);
  	
    if (!status.ok()) {
        cout << "Error opening database." << endl;
        cerr << status.ToString() << endl;
        exit(1);
    }

  	leveldb::WriteOptions wOptions;

    // Record Start Time 
    cout << "Start the timer" << endl;
    struct timeb _t;
    ftime(&_t);
    long start = _t.time * 1000 + _t.millitm;

  	// Process the input CSV file and construct the B+ tree index
	ifstream data; 
	data.open(inputFile);
	string line;

	while(getline(data,line)) {

		stringstream lineStream(line); // lineStream is used to process the record read from the csv fil
		string dataField; // contains the attribute of the record.

		getline(lineStream,dataField,',');

		// Since the record has only one attribute, the value field will be null.
		leveldb::Slice key = dataField.c_str();
		leveldb::Slice value = "";

		db->Put(wOptions, key, value);
		numRecs++;
	}

	// Print all key/value pairs in a database to terminal
	if (DEBUG_TERM) {

		leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
  		for (it->SeekToFirst(); it->Valid(); it->Next()) {
    		cout << it->key().ToString() << ": "  << it->value().ToString() << endl;
    		fwrite()
  		}

	  	assert(it->status().ok());  // Check for any errors found during the scan
  		delete it;

	}

	// Print all key/value pairs in a database to a file called "b+Index"
	if (DEBUG_FILE) {
		cout << "Print key/value pairs to b+index file!" << endl;
		string fileName = "b+Index";
		FILE *bIndex = fopen(fileName, "w");
		if (bIndex) {
			cout << "Error: Could not open the b+Index file!" << endl;
			goto stop_timer;
		}

		leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
  		for (it->SeekToFirst(); it->Valid(); it->Next()) {
    		cout << it->key().ToString() << ": "  << it->value().ToString() << endl;
    		fwrite(it->key().ToString(), sizeof(char), strlen(it->key().ToString()), fileName);
    		fwrite(":", sizeof(char), 1, fileName);
    		fwrite(it->value().ToString(), sizeof(char), strlen(it->value().ToString()), fileName);
    		fwrite("\n", sizeof(char), 1, fileName);
  		}

     	fclose(fileName);
	  	assert(it->status().ok());  // Check for any errors found during the scan
  		delete it;
	}

stop_timer:
    // Stop the timer
    cout << "Stop the timer" << endl;
    ftime(&_t);
    long finish = _t.time * 1000 + _t.millitm;
    long _time = finish - start;

    cout << "NUMBER OF RECORDS : " << numRecs << endl;
    cout << "TIME : " << _time << " milliseconds" << endl;

	delete db;
	return 0;
}
