#include <cstdio>

#define RECORD_LEN 9

typedef char* Record; // length is 9

typedef unsigned long Offset;

/**
 * creates runs of length `run_length` in
 * the `out_fp`.
 * The length of the run must be a multiple of 9
 */
void mk_runs(FILE *in_fp, FILE *out_fp, Offset run_length);