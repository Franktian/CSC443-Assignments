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

int main(int argc, char **argv) {


    /*if (argc != 3) {
		cout << "ERROR: invalid input parameters!" << endl;
		cout << "Usage: <parsed_biographies> <index_name>" << endl;
		exit(1);
	}*/

	ifstream file;

	file.open("parsefrank");

	if (!file) {
		cout << "Read file failed" << endl;
		exit(0);
	}

	string line;

	char dbname[] = "indexer";




	try {
		Xapian::WritableDatabase db(dbname, Xapian::DB_CREATE_OR_OPEN);
		Xapian::Document doc;

		// Record Start Time 
	    cout << "Start the timer" << endl;
	    struct timeb _t;
	    long total = 0;
	    ftime(&_t);
	    long start = _t.time * 1000 + _t.millitm;
	    


		while (getline(file, line)) {
			// First line must be a person's name, copy it
			char *name = new char [line.size() + 1];
			std::copy(line.begin(), line.end(), name);
			name[line.size()] = '\0';

			// cast name to a string
			string person_name(name, line.size() + 1);


			// The line following it is the biography, load it

			getline(file, line);



			istringstream iss(line);

			// Tokenize the biography

			vector<string> tokens;
			copy(istream_iterator<string>(iss),
	         istream_iterator<string>(),
	         back_inserter<vector<string> >(tokens));



			// This is to check if term already exists
			vector<string> terms;


			// Add terms to this person
			for (vector<string>::iterator it = tokens.begin() ; it != tokens.end(); ++it) {
				if (find(terms.begin(), terms.end(), *it) == terms.end()) {
					// If we do not have that term, we insert it
					doc.add_term(*it);
					terms.push_back(*it);
				}
			}

			// Set value and data
			doc.add_value(0, person_name);
			doc.set_data(line);
			db.add_document(doc);
			

			

			// Clear the document ready for the next
			doc.clear_terms();
			doc.clear_values();

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