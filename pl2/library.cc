#include "library.h"
#include <cassert>
#include <cstdlib>
#include <cstring>

/** 
 * Read a chunk from a file, at the position given by offset
 */
Offset _read_chunk_from_file(char* chunk, Offset start,
                            Offset chunk_size, FILE *file_ptr) {
    fseek(file_ptr, start, SEEK_SET);
    return fread(chunk, chunk_size, 1, file_ptr);
}

/** 
 * Write a chunk to a file, at the position given by offset 
 * set start_begin to false to start the offset at the end of the file
 */
void _write_chunk_to_file(char* chunk, Offset start, Offset chunk_size, 
                        FILE *file_ptr, bool start_begin = true) {
    fseek(file_ptr, start, start_begin ? SEEK_SET : SEEK_END);
    fwrite(chunk, chunk_size, 1, file_ptr);
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
void mk_runs(FILE *in_fp, FILE *out_fp, Offset run_length) {
    // First check if the length of the run is a multiple of 9
    assert(run_length % RECORD_LEN == 0);

    // Allocate memory for run buffer
    char* run_buf = new char[run_length];

    Offset file_size = _get_eof_offset(in_fp);
    int num_run = file_size / run_length + 1;
    unsigned long num_records_per_run = run_length / RECORD_LEN;
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
    delete run_buf;
}