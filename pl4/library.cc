#include <xapian.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <iterator>
#include <sys/timeb.h>

using namespace std;

/**
 * Tokenizer for a singe string with no space
 */
vector<string> ngram_tokenizer (string to_be_tokenized, int n) {
	int length = to_be_tokenized.length();
	vector<string> tokens;
	string sub;
	if (length > n) {
		for (int i = 0; i < length - n + 1; i++) {
			sub = to_be_tokenized.substr(i, n);
			tokens.push_back(sub);
		}
	} else {
		tokens.push_back(to_be_tokenized);
	}
	return tokens;
}
/**
 * Tokenize a given string by white space
 */
vector<string> space_tokenizer (string to_be_tokenized) {
	istringstream iss(to_be_tokenized);

	vector<string> tokens;
	copy(istream_iterator<string>(iss),
	         istream_iterator<string>(),
	         back_inserter<vector<string> >(tokens));
	return tokens;
}

int main(int argc, char **argv) {
	string frank = "a b c d e";
	vector<string> token = space_tokenizer(frank);
	ngram_tokenizer("ab", 3);
}