#include <xapian.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <sys/timeb.h>

using namespace std;

void replace(string& str, const string& from, const string& to) {
    if(from.empty()) { return; }
    size_t start_pos = 0;
    size_t to_length = to.length();
    while((start_pos = str.find(from, start_pos)) != string::npos) {

    	// Check to make sure the term is not a part of another word
		if (start_pos > 0 && str.at(start_pos-1) != ' ') {
			continue;
		}
		if (start_pos+to_length != string::npos && str.at(start_pos+to_length+1) != ' ') {
			continue;
		}
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

int main(int argc, char **argv) {
	if (argc < 4) {
        cout << "ERROR: invalid input parameters!" << endl;
        cout << "bio_search <index_name> <top-k> <keyword1> <keyword2> ..." << endl;
        exit(1);
    }
    char* index_name = (char*)argv[1];
    int k = atoi((argv[2]));
    int num_search_terms = argc - 3;
    if (k < 1) {
        cout << "top-k must be at least 1!" << endl;
        exit(1);
    }

    // Start timer
    cout << "Start the timer" << endl;
    struct timeb _t;
    ftime(&_t);
    long start = _t.time * 1000 + _t.millitm;
    
    try {
	    // Open the database
	    Xapian::Database db(index_name);
	    
	    // Construct the query terms
	    vector<string> query_terms;
	    for (int i = 3; i < argc; ++i) {
	    	query_terms.push_back(argv[i]);
	    }
	    // Pass in operator and vector iterators
		Xapian::Query query(Xapian::Query::OP_OR, query_terms.begin(), query_terms.end());
		cout << "Performing query `" << query.get_description() << "`" << endl;

		// Run the query
		Xapian::Enquire enquire(db);
		enquire.set_query(query);
		Xapian::MSet matches = enquire.get_mset(0, k);
		cout << "Result mset size is " << matches.size() << endl;

		// Prepare the highlighted search terms
		vector<string> highlighted_terms;
		for (vector<string>::iterator it = query_terms.begin(); 
				it != query_terms.end(); ++it)
		{
			string highlight = "====" + (*it) + "====";
			highlighted_terms.push_back(highlight);
		}

		// Show the result
		Xapian::MSetIterator i;
		for (i = matches.begin(); i != matches.end(); ++i) {
			// Print the document ID and matching metrics
			cout << "Document ID " << *i << "\t";
			cout << i.get_percent() << "% \t";
			Xapian::Document doc = i.get_document();

			// Print the person name
			cout << doc.get_value(0) << endl;

			// Get the original document and highlight the search terms
			string data = doc.get_data();
			for (int j = 0; j < num_search_terms; j++) {
				replace(data, query_terms.at(j), highlighted_terms.at(j));
			}

			// Print the highlighted document
			cout << "[" << data << "]" << endl;
		}

    } catch (const Xapian::Error & error) {
		cout << "Exception: " << error.get_msg() << endl;
	}

    // Stop the timer
    cout << "Stop the timer" << endl;
    ftime(&_t);
    long finish = _t.time * 1000 + _t.millitm;
    cout << "TIME : " << finish - start << " milliseconds" << endl;
	return 0;
}