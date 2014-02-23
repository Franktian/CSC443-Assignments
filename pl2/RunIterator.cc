RunIterator::RunIterator(FILE *fp, long start_pos, long run_length, long buf_size) {
	this->curr_pos = 0;
	this->run_length = run_length;
	this->buf = (char *)malloc(buf_size);
}

Record RunIterator::next() {
	if (!this->hasNext()) {
		return NULL;
	}
}

bool RunIterator::hasNext() {
	return this->curr_pos >= this->run_length;
}