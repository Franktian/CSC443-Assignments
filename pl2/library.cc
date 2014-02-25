#include "library.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

using namespace std;

/** 
 * Read a chunk from a file, at the position given by offset
 */
Offset _read_chunk_from_file(char* chunk, Offset start,
                            Offset chunk_size, FILE *file_ptr) {
    fseek(file_ptr, start, SEEK_SET);
    // cout << "CHUNK SIZE " << chunk_size << endl;
    int y = fread(chunk, 1, chunk_size, file_ptr);
    // cout << y << endl;
    return y;
}

/** 
 * Write a chunk to a file, at the position given by offset 
 * set start_begin to false to start the offset at the end of the file
 */
void _write_chunk_to_file(char* chunk, Offset start, Offset chunk_size, 
                        FILE *file_ptr, bool start_begin = true) {
    fseek(file_ptr, start, start_begin ? SEEK_SET : SEEK_END);
    fwrite(chunk, 1, chunk_size, file_ptr);
    fflush(file_ptr);
}

/**
 * Get the position at the end of the file 
 */
Offset _get_eof_offset(FILE* file) {
    fseek(file, 0, SEEK_END);
    return ftell(file);
}

int _record_compare(void const *r1, void const *r2) { 
    char const *x = (char const *)r1;
    char const *y = (char const *)r2;
    return strcmp(x, y);
}

/**
 * creates runs of length `run_length` in
 * the `out_fp`.
 */
int mk_runs(FILE *in_fp, FILE *out_fp, Offset run_length) {
    // First check if the length of the run is a multiple of 9
    assert(run_length % RECORD_LEN == 0);

    // Allocate memory for run buffer
    char* run_buf = new char[run_length];

    Offset file_size = _get_eof_offset(in_fp);
    int num_run = file_size / run_length + 1;
    long num_records_per_run = run_length / RECORD_LEN;
    Offset last_run_length = file_size % run_length;
    
    int curr_run = 0;
    // Read, sort and write out complete runs
    while (curr_run < num_run - 1) {
        // Read
        _read_chunk_from_file(run_buf, curr_run*run_length, run_length, in_fp);
        // Sort
        qsort(run_buf, num_records_per_run, RECORD_LEN, _record_compare);
        // Write
        _write_chunk_to_file(run_buf, curr_run*run_length, run_length, out_fp);
        curr_run++;
    }
    // Process the last incomplete run
    if (last_run_length > 0) {
        // Read
        _read_chunk_from_file(run_buf, curr_run*run_length, last_run_length, in_fp);
        // Sort
        qsort(run_buf, num_records_per_run, RECORD_LEN, _record_compare);
        // Write
        _write_chunk_to_file(run_buf, curr_run*run_length, last_run_length, out_fp);
    }

    // Finish and free memory
    delete[] run_buf;

    return last_run_length > 0 ? num_run : num_run - 1;
}

///////////////Frank//////////////


RunIterator::RunIterator(FILE *fp, long start_pos, long run_length, long buf_size) {
    
    // cout << "Creating run iterator at file position " << start_pos << endl;

    this->read_start = start_pos;
    this->read_end = start_pos + run_length;
    this->read_curr = this->read_start;
    this->read_length = run_length;

    this->buf = new char[buf_size];
    this->record = new char[RECORD_LEN];

    this->buf_start = 0;
    this->buf_end = buf_size;
    this->buf_curr = 0;
    this->buf_length = buf_size;

    this->fp = fp;

    this->buf_end = buf_length >= run_length ? run_length : buf_length;
    _read_chunk_from_file(buf, read_curr, buf_end - buf_start, fp);
}

RunIterator::~RunIterator() {
    delete[] this->buf;
    delete[] this->record;
}

Record RunIterator::next() {

    if (this->buf_curr >= this->buf_end) {
        if (read_curr < read_end) {
            this->buf_curr = 0;
            if (this->buf_length >= this->read_end - this->read_curr) {
                this->buf_end = this->read_end - this->read_curr;
                _read_chunk_from_file(buf, read_curr, this->buf_end, fp);
                // cout << buf << endl;
            } else {
                _read_chunk_from_file(buf, read_curr, this->buf_length, fp);
                // cout << buf << endl;
            }
        } else {
            return NULL;
        }
    }
    memcpy(this->record, (char *)(this->buf + this->buf_curr), RECORD_LEN);
    // cout << "Record: " << (char*)this->record << endl;
    this->buf_curr += RECORD_LEN;
    this->read_curr += RECORD_LEN;
    return this->record;
}

///////////////Frank//////////////




bool _not_all_null(char** records, int size) {
    for (int i = 0; i < size; ++i)
    {
        if (records[i] != NULL) {
            return true;
        }
    }
    return false;
}

int _find_min(char** records, int size) {
    int i = 0;
    while (records[i] == NULL && i < size) { i++; }
    // If all of the elements are NULL then we are using this function incorrectly
    assert(i < size);
    // Set the min initially to the first element that is not NULL
    int min_indx = i;
    while (i < size) {
        // Skip the NULL element
        if (records[i] == NULL) { i++; continue; }
        if (strcmp(records[min_indx], records[i]) > 0) {
            min_indx = i;
        }
        i++;
    }
    return min_indx;
}

void _merge(FILE* in_fp, FILE* out_fp, Offset merge_start, Offset merge_size, Offset output_start, long run_length, int num_runs, long buf_size) {
    // First check if the length of the run is a multiple of 9
    assert(run_length % RECORD_LEN == 0);
    assert(buf_size % RECORD_LEN == 0);

    char* out_buf = new char[buf_size];
    char** top_buf = new char*[num_runs];
    for (int i = 0; i < num_runs; ++i)
    {
        top_buf[i] = new char[RECORD_LEN];
    }

    RunIterator** iterators = new RunIterator*[num_runs];
    
    Offset last_run_length = merge_size % run_length;

    cout << "Merging " << num_runs << " runs, with total size " << merge_size << endl;
    // cout << "The last in this merge is size " << last_run_length << endl;

    int curr_run = 0;
    // iterate over complete runs to initialize the run iterators
    while (curr_run < num_runs - 1) {
        iterators[curr_run] = new RunIterator(in_fp, merge_start+curr_run*run_length, run_length, buf_size);
        curr_run++;
    }
    // make sure the last run is constructed with proper length
    if (last_run_length > 0) {
        iterators[curr_run] = new RunIterator(in_fp, merge_start+curr_run*run_length, last_run_length, buf_size);
    } else {
        iterators[curr_run] = new RunIterator(in_fp, merge_start+curr_run*run_length, run_length, buf_size);
    }

    // Merging
    // initialize top buff
    for (int i = 0; i < num_runs; ++i)
    {
        // Copy the content instead of assigning pointer
        memcpy(top_buf[i], iterators[i]->next(), RECORD_LEN);
    }

    long curr_records_in_buf = 0;
    long max_records_in_buf = buf_size / RECORD_LEN;
    Offset curr_pos = output_start;

    // Stop only when all run iterators are exhausted
    while (_not_all_null(top_buf, num_runs)) {
        // Get the minimum record
        int min_indx = _find_min(top_buf, num_runs);
        // Append to the output buffer
        memcpy(out_buf+curr_records_in_buf*RECORD_LEN, top_buf[min_indx], RECORD_LEN);
        curr_records_in_buf ++;
        if (curr_records_in_buf == max_records_in_buf) {
            // Write the output buffer to file if it is full
            _write_chunk_to_file(out_buf, curr_pos, buf_size, out_fp);
            curr_pos += buf_size;
            curr_records_in_buf = 0;
        }
        top_buf[min_indx] = iterators[min_indx]->next();
    }
    // Write the reminder of the buffer to file
    if (curr_records_in_buf > 0) {
        _write_chunk_to_file(out_buf, curr_pos, curr_records_in_buf*RECORD_LEN, out_fp);
    }

    // Clean up
    delete[] iterators;
    delete[] out_buf;
    for (int i = 0; i < num_runs; ++i)
    {
        delete[] top_buf[i];
    }
    delete[] top_buf;
}

long _ceil(long divided, long divisor) {
    long ans = divided / divisor;
    long rmd = divided % divisor;
    return rmd > 0 ? ans + 1 : ans;
}

/**
 * Merge runs with given run length
 */
int merge_runs(FILE* in_fp, FILE *out_fp, long run_length, int k, long buf_size) {
    // First check if the length of the run is a multiple of 9
    assert(run_length % RECORD_LEN == 0);
    assert(buf_size % RECORD_LEN == 0);

    Offset file_size = _get_eof_offset(in_fp);

    int num_runs = _ceil(file_size, run_length);
    cout << "Current number of runs: " << num_runs << ", size: " << run_length << " bytes" << endl;
    int num_merges = _ceil(num_runs, k);
    int last_merge_num_runs = num_runs % k;

    int curr_merge = 0;
    int file_pos = 0;
    // The complete k-way merges
    while (curr_merge < num_merges - 1) {
        _merge(in_fp, out_fp, file_pos, run_length*k, file_pos, run_length, k, buf_size);
        // cout << "Merging " << k << " runs of total size " << run_length*k << endl;
        file_pos += run_length*k;
        curr_merge ++;
    }
    // The last merge run may not always be k-way
    Offset merge_size = file_size - file_pos;
    if (last_merge_num_runs > 0) {
        _merge(in_fp, out_fp, file_pos, merge_size, file_pos, run_length, last_merge_num_runs, buf_size);
        // cout << "Merging " << last_merge_num_runs << " runs of total size " << merge_size << endl;
    } else {
        _merge(in_fp, out_fp, file_pos, merge_size, file_pos, run_length, k, buf_size);
        // cout << "Merging " << k << " runs of total size " << merge_size << endl;
    }
    return num_merges;
}