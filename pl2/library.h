#include <cstdio>

typedef char* Record; // length is 9

/**
 * creates runs of length `run_length` in
 * the `out_fp`.
 */
void mk_runs(FILE *in_fp, FILE *out_fp, long run_length);