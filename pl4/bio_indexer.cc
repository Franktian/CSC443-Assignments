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

	//int *gram_size = (int *)argv[3];
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
	    


		while (getline(file, line)) {
			// First line must be a person's name, copy it
			char *name = new char [line.size() + 1];
			std::copy(line.begin(), line.end(), name);
			name[line.size()] = '\0';

			// cast name to a string
			string person_name(name, line.size() + 1);

			// The line following it is the biography, load it

			getline(file, line);



			//istringstream iss(line);

			// Tokenize the biography in to n gram
			// the line variable is a string need to be tokenized into ngram

			/*vector<string> tokens;
			copy(istream_iterator<string>(iss),
	         istream_iterator<string>(),
	         back_inserter<vector<string> >(tokens));*/



			// This is the tokenized n-gram
			vector<string> ngram;
			string sub;
			// do a 3 gram first
			
			for (int i = 0; i < line.length() - size + 1; i++) {
				//string sub;
				//cout << 'size: ' << *gram_size << endl;
				sub = line.substr(i, size);
				//cout << "i: " << i << endl;
				//cout << sub << endl;
				doc.add_term(sub);
				//ngram.push_back(sub);
			}


			// Add terms to this person
			/*for (vector<string>::iterator it = tokens.begin() ; it != tokens.end(); ++it) {
				if (find(terms.begin(), terms.end(), *it) == terms.end()) {
					// If we do not have that term, we insert it
					doc.add_term(*it);
					terms.push_back(*it);
				}
			}*/

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