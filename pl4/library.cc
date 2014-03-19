#include <xapian.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <cstring>
#include <iterator>
#include <sys/timeb.h>
#include <set>
#include "library.h"

/**
 * Compute Jaccard
 */
double get_jaccard_similarity (const set<string> &ngram1, const set<string> &ngram2) {
	int intersect = 0, union1 = 0;

	for (set<string>::iterator it1 = ngram1.begin(); it1 != ngram1.end(); ++it1) {
		for (set<string>::iterator it2 = ngram2.begin(); it2 != ngram2.end(); ++it2) {
			string a = *it1;
			string b = *it2;
			if (a.compare(b) == 0) {
				intersect++;
			}
		}
	}

	union1 = ngram1.size() + ngram2.size() - intersect;
	return (double)intersect / (double)union1;
}

/**
 * Get rid of the symbols of a given string
 */
string encrpytion (string to_be_encrpyted) {
	char chars[] = ",.<>/?;:'\"[]{}\\|-_=+()*&^\%$#@!`~";
	for (int i = 0; i < strlen(chars); ++i) {
		to_be_encrpyted.erase (std::remove(to_be_encrpyted.begin(), to_be_encrpyted.end(), chars[i]), to_be_encrpyted.end());
	}
	return to_be_encrpyted;
}

/**
 * Tokenizer for a string contains white spaces
 */
set<string> tokenizer (string to_be_tokenized, int n) {
	// Note that the input string is a string separate by white spaces
	set<string> result;
	// First, get rid of the symbols
	string encrpyed = encrpytion(to_be_tokenized);
	// Second tokenized the string by white space
	vector<string> tokens = space_tokenizer(encrpyed);
	// Second, get rid of the symbols
	for (vector<string>::iterator it = tokens.begin() ; it != tokens.end(); ++it) {
		// For each word in the string, get the n-gram
		vector<string> single = ngram_tokenizer(*it, n);
		for (vector<string>::iterator it2 = single.begin(); it2 != single.end(); ++it2) {
			result.insert(*it2);
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
