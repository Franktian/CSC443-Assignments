#include <iostream>
#include <stdlib.h>
#include <string.h>


class RunIterator {
public:
	/**
	 * creates an iterator using the 'buf_size' to
	 * scan through a run that starts at 'start_pos'
	 * with length 'run_length'
	 */
	RunIterator(FILE *fp, long start_pos, long run_length, long buf_size);
	~RunIterator();

	/**
	 * reads the next record. Returns null if
	 * iterator reads the end of the run.
	 */
	Record next();

	bool hasNext();

	long getRunLength();
private:
	int curr_pos;
	int run_length;
	char *buf;
};
void read_from_file(char *buf, long offset, FILE *fp, long length);