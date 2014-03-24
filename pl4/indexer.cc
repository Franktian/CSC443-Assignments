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


    if (argc != 5) {
		cout << "ERROR: invalid input parameters!" << endl;
		cout << "Usage: <parsed list file> <index name> <ngram size> <ngram unit length (3 for chinese, 1 for english)>" << endl;
		exit(1);
	}

	ifstream file;

	file.open(argv[1]);

	if (!file) {
		cout << "Read file failed" << endl;
		exit(0);
	}

	char *dbname = argv[2];

	//this indicate what gram you wouldl like to get
	int size = atoi(argv[3]);

	// The character length of each character in the ngram, 3 for chinese, 1 for english character
	int ngram_unit_len = atoi(argv[4]);

	try {
		Xapian::WritableDatabase db(dbname, Xapian::DB_CREATE_OR_OPEN);
		Xapian::Document doc;

		// Record Start Time 
	    cout << "Start the timer" << endl;
	    struct timeb _t;
	    long total = 0;
	    ftime(&_t);
	    long start = _t.time * 1000 + _t.millitm;

	    string line;  // Read the blank line
	    string firstline;
	    string secondline;
	    string thirdline;
	    string fourthline;
	    string fifthline;

		while (getline(file, line)) {
			// Readin the lines
			getline(file, firstline);
			getline(file, secondline);
			getline(file, thirdline);
			getline(file, fourthline);
			getline(file, fifthline);

			// combine all lines into a single data
			string data = firstline + "\n" + secondline + "\n" + thirdline + "\n" + fourthline + "\n" + fifthline;
			// cout << data << endl;
			set<string> tokens1 = ngram_tokenizer(firstline, size, ngram_unit_len);
			set<string> tokens2 = ngram_tokenizer(secondline, size, ngram_unit_len);
			set<string> tokens3 = ngram_tokenizer(thirdline, size, ngram_unit_len);
			set<string> tokens4 = ngram_tokenizer(fourthline, size, ngram_unit_len);
			set<string> tokens5 = ngram_tokenizer(fifthline, size, ngram_unit_len);

			for (set<string>::iterator it1 = tokens1.begin(); it1 != tokens1.end(); ++it1) {
				doc.add_term(*it1);
			}
			for (set<string>::iterator it2 = tokens2.begin(); it2 != tokens2.end(); ++it2) {
				doc.add_term(*it2);
			}
			for (set<string>::iterator it3 = tokens3.begin(); it3 != tokens3.end(); ++it3) {
				doc.add_term(*it3);
			}

			for (set<string>::iterator it4 = tokens4.begin(); it4 != tokens4.end(); ++it4) {
				doc.add_term(*it4);
			}

			for (set<string>::iterator it5 = tokens5.begin(); it5 != tokens5.end(); ++it5) {
				doc.add_term(*it5);
			}

			// Add the documents to the database
			doc.add_value(0, fifthline);
			doc.set_data(data);
			db.add_document(doc);

			// Clear the documents for reuse
			doc.clear_terms();
			doc.clear_values();
		}

		// Commit the changes to the database
		db.commit();

		// Record the time
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