#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <cstring>
#include <math.h>

#include <sys/timeb.h>

using namespace std;

int main( int argc, const char* argv[] )
{
    if (argc != 5) {
        cout << "ERROR: invalid input parameters!" << endl;
        cout << "Please enter <input_file> <output_file> <mem_capacity> <k>" << endl;
        exit(1);
    }

    char* input_file = (char*)argv[1];
    char* output_file = (char*)argv[2];
    long mem_capacity = atoi((argv[3]));
    long k = atoi((argv[4]));

    // Check if the mem_capacity is large enough
    if (RECORD_LEN > mem_capacity/k) {
        cout << "mem_capacity is too small!" << endl;
        exit(1);
    }

    // Record Start Time 
    cout << "Start the timer" << endl;
    struct timeb _t;
    ftime(&_t);
    long start = _t.time * 1000 + _t.millitm;

    FILE *inputFile = fopen(input_file, "r");
    if (!inputFile) {
        cout << "Error: Cannot open the input file!" << endl;
        exit(1);
    }

    FILE *outputFile = fopen(output_file, "w");
    if (!outputFile) {
        cout << "Error: Cannot open the output file!" << endl;
        exit(1);
    }

    // Determine how many records to be processed
    fseek(inputFile, 0L, SEEK_END);
 	long numRecs = ftell(inputFile)/RECORD_LEN;
	fseek(inputFile, 0L, SEEK_SET);
    cout << "File to sort has " << numRecs << " records";

    FILE *temp_out = fopen("temp.out1", "w");
    if (!temp_out) {
        cout << "Error: Cannot open the temp output file!" << endl;
        exit(1);
    }

    // Determine the buffer size and make sure it is multiple of the length of record
    long buf_sz = floor(mem_capacity/(k+1));
    long adjustment = buf_sz % RECORD_LEN;
    if (adjustment > 0)
        buf_sz -= adjustment;

    // Lets let run length based on the buf_sz
    int run_length = buf_sz;

    // Make runs with each run having size run_length
    int num_runs = mk_runs(inputFile, outputFile, run_length);

    // Merge Sort iterations
    while (num_runs > 1) {
        num_runs = merge_runs(outputFile, temp_out, run_length, k, buf_sz);
        if (num_runs < 0) {
            cout << "Error: Something wrong in the merge_runs()!" << endl;
            exit(1);
        }
        // now temp_out has the output file
        // swap temp file pointers
        FILE* swap_temp = temp_out;
        temp_out = outputFile;
        outputFile = swap_temp;
        // now outputFile has the output file
    }
    // The final output is always in the correct file (outputFile)

    // Stop the timer
    cout << "Stop the timer" << endl;
    ftime(&_t);
    long finish = _t.time * 1000 + _t.millitm;
    long _time = finish - start;

    cout << "NUMBER OF RECORDS : " << numRecs << endl;
    cout << "TIME : " << _time << " milliseconds" << endl;

    fclose(inputFile);
    fclose(outputFile);
    fclose(temp_out);
  	return 0;
}
