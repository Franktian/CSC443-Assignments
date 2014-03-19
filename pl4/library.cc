#include <xapian.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <iterator>
#include <sys/timeb.h>
#include "library.h"


/**
 * Tokenizer for a string contains white spaces
 */
vector<string> tokenizer (string to_be_tokenized, int n) {
	// Note that the input string is a string separate by white spaces
	vector<string> result;
	// First tokenized the string by white space
	vector<string> tokens = space_tokenizer(to_be_tokenized);
	for (vector<string>::iterator it = tokens.begin() ; it != tokens.end(); ++it) {
		// For each word in the string, get the n-gram
		vector<string> single = ngram_tokenizer(*it, n);
		for (vector<string>::iterator it2 = single.begin(); it2 != single.end(); ++it2) {
			result.push_back(*it2);
		}
		//cout << "****" << endl;
	}
	return result;
}

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
	string frank = "microsoft frank";
	vector<string> token = space_tokenizer(frank);
	vector<string> result = tokenizer(frank, 3);
	for (vector<string>::iterator it = result.begin(); it != result.end(); ++it) {
		cout << *it << endl;
	}
}