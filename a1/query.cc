#include <sstream>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <set>
#include <string>
#include <string.h>
#include <sys/timeb.h>

#include "library.h"

#define RECORD_SIZE 1000
#define SUBSTRING_LEN 5
#define QUERY_DEBUG 0

using namespace std;

int main( int argc, const char* argv[] )
{
    cout << "Start the timer" << endl;
    struct timeb _t;                
    ftime(&_t);
    long startt = _t.time * 1000 + _t.millitm; 
    // Process the input parameters.
    if (argc != 5){
	cout << "ERROR: invalid input parameters!" << endl;
        cout << "Please put: <heapfile> <start> <end> <page_size>" << endl;
        exit(1);
     }

    const char* heapfile_name = argv[1];
    char* start = (char*)argv[2];
    char* end = (char*)argv[3];
    int page_size = atoi((argv[4]));
    set<string> dataSet;

    /* SQL: 
     * SELECT SUBSTRING(A2, 1, 5), COUNT(*) FROM T
     * WHERE A1 >= start AND A1 <= end
     * GROUP BY SUBSTRING(A2, 1, 5) 
     */

    // Open the heapfile and extract data
    FILE *hFile = fopen(heapfile_name, "r");
    if (!hFile) {
	cout << "ERROR: failed to open the heapfile!" << endl;
	exit(1);
    }    

    Heapfile *heapfile = new Heapfile();
    heapfile->page_size = page_size;
    heapfile->file_ptr = hFile;

    // Prepare records
    Record currentRecord;
    for(int i = 0; i < SCHEMA_ATTRIBUTES_NUM; i++){
        char content[10] = "         ";
        currentRecord.push_back(content);
    }

    RecordIterator* iterator = new RecordIterator(heapfile);

    FILE *T = fopen("tempView", "w+");
    if (!T) {
	cout << "ERROR: Failed to open the tempView file!" << endl;
	exit(1);
    }	

    int numRec = 0;
    while(iterator->hasNext()) {
        currentRecord = iterator->next();
        // A1 is the first attribute A1 = currentRecord.at(0)
        // WHERE Clause Processing: A1 >= start AND A1 <= end
        if ((currentRecord.at(0))[0] >= start[0] && (currentRecord.at(0))[9] <= end[0]) {
            // A2 is the second attribute A2 = currentRecord.at(1)
            // Save satisfying data to the tempView file 
            fwrite(&currentRecord.at(1)[1], sizeof(char), 1, T);
            fwrite(&currentRecord.at(1)[2], sizeof(char), 1, T);
            fwrite(&currentRecord.at(1)[3], sizeof(char), 1, T);
            fwrite(&currentRecord.at(1)[4], sizeof(char), 1, T);
            fwrite(&currentRecord.at(1)[5], sizeof(char), 1, T);
        }
        numRec++;
    }

    // Close the heap file since all the records have been processed.
    // The ones satisfy the where clause have been written the temp file.
    fclose(hFile);

    // Process the "Group By" Clause.
    // Get how many bytes in the temp view file
    long T_length = ftell(T);
    char *substring = new char[SUBSTRING_LEN];
    rewind(T);
  
    FILE *temp = fopen("output.view", "w+");

    int total_read = 0;
    while (total_read < T_length) {
	
        // Get the substring used for "Group By"
        fread(substring, sizeof(char), SUBSTRING_LEN, T);
        long cursor_position = ftell(T);

        // Check whether the substring has been processed before.
        if (dataSet.count(substring)) {
	    // substring is in the set, count is 1
            // The substring has been processed.
	    if (QUERY_DEBUG) 
            cout << "The substring < " << substring << " > has been processed!" << endl;
        fseek(T, cursor_position, SEEK_SET);
        total_read+=SUBSTRING_LEN;
        continue;   
	} 

        if (QUERY_DEBUG) cout << "Prossing data : " << substring << endl;
        dataSet.insert(substring); 

        // Count the number of occurance
        rewind(T);
        int bytesProcessed = 0;
        int count = 0;

        while (bytesProcessed < T_length) {

            char* comparingString = new char[SUBSTRING_LEN];
            // Read the comparing string from the view file.
            fread(comparingString, sizeof(char), SUBSTRING_LEN, T); 
            // Compare the substring and check whether the substring has been processed before.
            if (!strcmp(substring, comparingString)) {
		if(QUERY_DEBUG) cout << "Occurrence Happened!" << endl;
                count++;
            }

            bytesProcessed += SUBSTRING_LEN;
            delete comparingString;
        }

        // Write to a temp file
	char count_string[32];
        sprintf(count_string, "%d", count);
        fwrite(substring, sizeof(char), SUBSTRING_LEN, temp);
        fwrite(" ", sizeof(char), 1, temp);
	fwrite(count_string, sizeof(char), strlen(count_string), temp);
        fwrite("\n", sizeof(char), 1, temp);
        // cout << "Substring is: " << substring << endl;
        // cout << "Occurrence Count So Far: " << count << endl;

        fseek(T, cursor_position, SEEK_SET);
        total_read+=SUBSTRING_LEN;
    }

    delete substring;
    fclose(temp);
    fclose(T);
    cout << "NUMBER OF RECORDS : " << numRec << endl;

    cout << "Stop the timer" << endl;
    ftime(&_t);
    long finish = _t.time * 1000 + _t.millitm;
    long _time = finish - startt;
    cout << "TIME : " << _time << " milliseconds" << endl;
    return 0;
}
