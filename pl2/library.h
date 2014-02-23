#include <cstdio>

#define RECORD_LEN 9

typedef char* Record; // length is 9

typedef long Offset;

/**
 * creates runs of length `run_length` in
 * the `out_fp`.
 * The length of the run must be a multiple of 9
 */
void mk_runs(FILE *in_fp, FILE *out_fp, Offset run_length);

class RunIterator {
public:
    /**
     * creates an iterator using the `buf_size` to
     * scan through a run that starts at `start_pos`
     * with length `run_length`
     */
    RunIterator(FILE *fp, long start_pos, long run_length, long buf_size);
    /**
     * reads the next record.  Returns null if
     * iterator reads the end of the run.
     */
    Record next();
};

/**
 * Return the number of runs remains after the merge runs
 */
int merge_runs(FILE* in_fp, FILE *out_fp, long run_length, int k, long buf_size);