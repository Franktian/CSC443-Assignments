#include <library.h>


RunIterator::RunIterator(FILE *fp, long start_pos, long run_length, long buf_size) {
	this->curr_pos = 0;
	this->run_length = run_length;
	this->buf = (char *)malloc(buf_size);
	read_from_file(this->buf, start_pos, fp, run_length);
}

RunIterator::~RunIterator() {
	delete this->buf;
}

Record* RunIterator::next() {
	if (!this->hasNext()) {
		return NULL;
	}

	Record *record;
	strncpy((char *)record, (const char *)this->buf[this->curr_pos], RECORD_LEN);
	this->curr_pos += RECORD_LEN;
	return record;
}

bool RunIterator::hasNext() {
	return this->curr_pos >= this->run_length;
}

long RunIterator::runLength() {
	return this->run_length;
}

void read_from_file(char *buf, long offset, FILE *fp, long length) {
	fseek(fp, offset, SEEK_SET);
	fread(buf, sizeof(char), length, fp);
}