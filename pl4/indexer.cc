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
 
using namespace std;


int main(int argc, char **argv) {


    if (argc != 4) {
		cout << "ERROR: invalid input parameters!" << endl;
		cout << "Usage: <parsed_biographies> <index_name> <n>" << endl;
		exit(1);
	}

	ifstream file;

	file.open(argv[1]);

	if (!file) {
		cout << "Read file failed" << endl;
		exit(0);
	}

	string line;

	char *dbname = argv[2];

	//this indicate what gram you wouldl like to get
	int size = atoi(argv[3]);

	try {
		Xapian::WritableDatabase db(dbname, Xapian::DB_CREATE_OR_OPEN);
		Xapian::Document doc;

		// Record Start Time 
	    cout << "Start the timer" << endl;
	    struct timeb _t;
	    long total = 0;
	    ftime(&_t);
	    long start = _t.time * 1000 + _t.millitm;

	    string firstline;
	    string secondline;
	    string thirdline;
	    string fourthline;
	    string fifthline;
	    //bool canAddTerm1;
	    //bool canAddTerm2;


		while (getline(file, line)) {
			//cout << line << endl;
			//string firstline = line;
			/*canAddTerm1 = true;
			canAddTerm2 = true;
			canAddTerm3 = true;
			canAddTerm4 = true;
			canAddTerm5 = true;*/
			getline(file, firstline);
			getline(file, secondline);
			getline(file, thirdline);
			getline(file, fourthline);
			getline(file, fifthline);

			// combine all lines into a single data
			string data = firstline + "\n" + secondline + "\n" + thirdline + "\n" + fourthline + "\n" + fifthline;
			// cout << data << endl;
			set<string> tokens1 = ngram_tokenizer_modified(firstline, size);
			set<string> tokens2 = ngram_tokenizer_modified(secondline, size);
			set<string> tokens3 = ngram_tokenizer_modified(thirdline, size);
			set<string> tokens4 = ngram_tokenizer_modified(fourthline, size);
			set<string> tokens5 = ngram_tokenizer_modified(fifthline, size);

			for (set<string>::iterator it1 = tokens1.begin(); it1 != tokens1.end(); ++it1) {
				//cout << *it1 << endl;
				doc.add_term(*it1);
				// cout << *it1 << endl;
			}
			for (set<string>::iterator it2 = tokens2.begin(); it2 != tokens2.end(); ++it2) {
				//cout << *it2 << endl;
				// cout << "term: " << *it2 << endl;

				doc.add_term(*it2);

			}
			for (set<string>::iterator it3 = tokens3.begin(); it3 != tokens3.end(); ++it3) {
				//cout << *it3 << endl;
				doc.add_term(*it3);
			}

			for (set<string>::iterator it4 = tokens4.begin(); it4 != tokens4.end(); ++it4) {
				//cout << *it4 << endl;
				doc.add_term(*it4);
			}

			for (set<string>::iterator it5 = tokens5.begin(); it5 != tokens5.end(); ++it5) {
				//cout << *it5 << endl;
				doc.add_term(*it5);
			}

			doc.add_value(0, fifthline);
			doc.set_data(data);
			db.add_document(doc);

			doc.clear_terms();
			doc.clear_values();
			/*
			//cout << "****" << endl;
			// First line must be a person's name, copy it
			char *name = new char [line.size() + 1];
			std::copy(line.begin(), line.end(), name);
			name[line.size()] = '\0';

			// cast name to a string
			string person_name(name, line.size() + 1);

			// The line following it is the biography, load it

			getline(file, line);

			// Tokenize the biography in to n gram
			// the line variable is a string need to be tokenized into ngram
			// This is the tokenized n-gram

			string sub;			
			for (int i = 0; i < line.length() - size + 1; i++) {
				sub = line.substr(i, size);
				//cout << sub << endl;
				doc.add_term(sub);
			}

			// Set value and data
			doc.add_value(0, person_name);
			doc.set_data(line);
			db.add_document(doc);

			// Clear the document ready for the next
			doc.clear_terms();
			doc.clear_values();*/

		}

		db.commit();
		ftime(&_t);
		long finish = _t.time * 1000 + _t.millitm;
		total = finish - start;

		cout << "TIME : " << total << " milliseconds" << endl;

	    
	} catch (const Xapian::Error & error) {
		cout << "Exception: " << error.get_msg() << endl;
	}

	

	file.close();

	return 0;

}