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

#define RECORD_SIZE 9


int main( int argc, const char* argv[] )
{
    if (argc != 5) {
        cout << "ERROR: invalid input parameters!" << endl;
        cout << "Please enter <input_file> <output_file> <mem_capacity> <k>" << endl;
        exit(1);
    }

    char* input_file = (char*)argv[1];
    char* output_file = (char*)argv[2];
    int mem_capacity = atoi((argv[3]));
    int k = atoi((argv[4]));

    // Check if the mem_capacity is large enough
    if (RECORD_SIZE > mem_capacity/k) {
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

    FILE *outputFile = fopen(output_file, "w+");
    if (!outputFile) {
        cout << "Error: Cannot open the output file!" << endl;
        exit(1);
    }

    // Determine how many records to be processed
    fseek(inputFile, 0L, SEEK_END);
 	  int numRecs = ftell(inputFile)/RECORD_SIZE;
	  fseek(inputFile, 0L, SEEK_SET);

    FILE *temp_out = fopen("temp.out", "w");
    if (!temp_out) {
        cout << "Error: Cannot open the temp output file!" << endl;
        exit(1);
    }

    int run_length = 0;
    // Produce ceil(mem_capacity/(k+1)) numbers of sorted segments
    mk_runs(inputFile, temp_out, floor(mem_capacity/(k+1)));

    // Determine the buffer size and make sure it is multiple of 9
    int buf_sz = 0;
    int adjustment = (int)(floor(mem_capacity/(k+1))) % 9;
    if (adjustment)
        buf_sz = floor(mem_capacity/(k+1)) - adjustment;
    else
        buf_sz = floor(mem_capacity/(k+1));

    // Merge Sort iterations
    int ways = merge_runs(temp_out, outputFile, run_length, k, buf_sz);
    if (ways < 0) {
        cout << "Error: Something wrong in the merge_runs()!" << endl;
        exit(1);
    }

    while (ways > 1) {
        run_length *= k;
        ways = merge_runs(temp_out, outputFile, run_length, k, buf_sz);
        if (ways < 0) {
            cout << "Error: Something wrong in the merge_runs()! --- " << ways << endl;
            exit(1);
        }
    }

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
