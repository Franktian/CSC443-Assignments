#include <cstdio>

#define RECORD_LEN 8

typedef char* Record; // length is 8, as we don't read in null terminator

typedef long Offset;

/**
 * creates runs of length `run_length` in
 * the `out_fp`.
 * The length of the run must be a multiple of 9
 */
void mk_runs(FILE *in_fp, FILE *out_fp, Offset run_length);

class RunIterator {
public:
    long read_start;
    long read_end;
    long read_curr;
    long read_length;

    long buf_start;
    long buf_end;
    long buf_curr;
    long buf_length;
    char *buf;
    char *record;

    FILE *fp;
    /**
     * creates an iterator using the `buf_size` to
     * scan through a run that starts at `start_pos`
     * with length `run_length`
     */
    RunIterator(FILE *fp, long start_pos, long run_length, long buf_size);
    ~RunIterator();
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