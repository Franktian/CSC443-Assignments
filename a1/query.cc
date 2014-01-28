#include <sstream>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <set>
#include <sys/timeb.h>

#include "library.h"

#define RECORD_SIZE 1000
#define SUBSTRING_LEN 5

using namespace std;

int main( int argc, const char* argv[] )
{
  
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
    set<char*> dataSet;

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

    // *** RecordIterator* iterator = (RecordIterator*)malloc(sizeof(RecordIterator));
    RecordIterator* iterator = new RecordIterator(heapfile);
    // *** init_record_iterator(iterator, heapfile, RECORD_SIZE, page_size);

    FILE *T = fopen("tempView", "w+");
    if (!T) {
	cout << "ERROR: Failed to open the tempView file!" << endl;
	exit(1);
    }	

    int numRec = 0;
    // *** while(iterator->hasNext){
    while(iterator->hasNext()) {
        //*** iterate_record(iterator);  
        currentRecord = iterator->next();  
    
        // *** read_current_record(iterator, &cur);
        // A1 is the first attribute A1 = cur.at(0)
        // WHERE Clause Processing: A1 >= start AND A1 <= end
        if (currentRecord.at(0)[0] >= start[0] && currentRecord.at(0)[9] <= end[0]) {
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
  
    FILE *temp = fopen("_view", "w+");

    int total_read = 0;
    while (total_read < T_length) {
	
        //char* substring = new char[SUBSTRING_LEN];
        // Get the substring used for "Group By"
        fread(substring, sizeof(char), SUBSTRING_LEN, T);
        long cursor_position = ftell(T);

        // Check whether the substring has been processed before.
        if ((dataSet.find(substring) != dataSet.end())) {
	    // The substring has been processed.
            fseek(T, cursor_position, SEEK_SET);
            total_read+=SUBSTRING_LEN;
	    continue;    
        }
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
                count++;
            }

            bytesProcessed += SUBSTRING_LEN;
            delete comparingString;
        }

        // Write to a temp file
        fwrite(&substring, sizeof(char), SUBSTRING_LEN, temp);
        // fwrite(&count, sizeof(int), 1, temp);
        //printf("%s ", substring);
        //printf("%d\n", count);
        cout << "Substring is: " << substring << endl;
        cout << "Occurrence Count So Far: " << count << endl;

        fseek(T, cursor_position, SEEK_SET);
        total_read+=SUBSTRING_LEN;
        //delete substring;
    }

    delete substring;
    fclose(temp);
    fclose(T);
    printf("NUMBER OF RECORDS : %d\n", numRec);
    return 0;
}
