#include <xapian.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <sys/timeb.h>

using namespace std;

int main(int argc, char **argv) {
	if (argc < 4) {
        cout << "ERROR: invalid input parameters!" << endl;
        cout << "bio_search <index_name> <top-k> <keyword1> <keyword2> ..." << endl;
        exit(1);
    }
    char* index_name = (char*)argv[1];
    int k = atoi((argv[2]));
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
	    std::vector<string> query_terms;
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

		// Show the result
		Xapian::MSetIterator i;
		for (i = matches.begin(); i != matches.end(); ++i) {
			cout << "Document ID " << *i << "\t";
			cout << i.get_percent() << "% ";
			Xapian::Document doc = i.get_document();
			cout << doc.get_value(0) << endl;
			cout << "[" << doc.get_data() << "]" << endl;
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