#include <xapian.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <set>
#include <sys/timeb.h>
#include <cctype>
#include <cstdio>
#include <regex>
#include "library.h"

class JaccardMatchDecider : public Xapian::MatchDecider {

	set<string> query_terms;
	double similarity_threshold;

	public:
		JaccardMatchDecider(double similarity_threshold) {
			this->similarity_threshold = similarity_threshold;
		}
		void add_term(string value) {
			query_terms.insert(value);
		}
		bool operator()(const Xapian::Document& doc) const;
};

bool JaccardMatchDecider::operator()(const Xapian::Document& doc) const {
	Xapian::TermIterator i;
	std:set<string> doc_terms;
	for (i = doc.termlist_begin(); i != doc.termlist_end(); ++i)
	{
		doc_terms.insert(*i);
	}
	double similarity = get_jaccard_similarity_modified(query_terms, doc_terms);
	// cout << "similarity: " << similarity << endl;
	return  similarity >= similarity_threshold;
	//return false;
}

bool replace(string& str, const string& from, const string& to) {
    if(from.empty()) { return false; }
    bool found = false;
    size_t start_pos = 0;
    size_t to_length = to.length();
	size_t from_length = from.length();
    while((start_pos = str.find(from, start_pos)) != string::npos) {
    	// Check to make sure the term is not a part of another word
		// if (start_pos+from_length < str.length() && 
		// 	str.at(start_pos+from_length) != ' ') {
		// 	start_pos += from_length;
		// 	continue;
		// }
		// if (start_pos > 0 && str.at(start_pos-1) != ' ') {
		// 	start_pos += from_length;
		// 	continue;
		// }
		if (start_pos > 0 && str.at(start_pos-1) == '=') {
			start_pos += from_length;
			continue;
		}
		if (start_pos+from_length < str.length() &&
			str.at(start_pos+from_length) == '=') {
			start_pos += from_length;
			continue;
		}
        str.replace(start_pos, from_length, to);
        start_pos += to_length;
        found = true;
    }
    return found;
}

bool replace_asian_text(string& str, const string& from, const string& to) {
	if(from.empty()) { return false; }
    bool found = false;
    size_t start_pos = 0;
    size_t to_length = to.length();
	size_t from_length = from.length();
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from_length, to);
        start_pos += to_length;
        found = true;
    }
    return found;
}

bool fuzzy_highlight(string& data, const string& query_word, int ngram_length, 
		int ngrams_unit_length, double similarity_threshold) {
	double similarity;
	bool found = false;
	// For Asian text we just do simple replace, as regular expression cannot find word
	if (ngrams_unit_length == 3) {
		string highlight_word("===="+query_word+"====");
		return replace_asian_text(data, query_word, highlight_word);
	}
	set<string> query_ngrams = ngram_tokenizer(query_word, ngram_length, ngrams_unit_length);
	regex rgx("\\w+");
	for (sregex_iterator it(data.begin(), data.end(), rgx), it_end; it != it_end; ++it) {
		string word((*it)[0]);
		set<string> word_ngrams = ngram_tokenizer(word, ngram_length, ngrams_unit_length);
		similarity = get_jaccard_similarity(query_ngrams, word_ngrams);
		string highlight_word = "====" + word + "====";
		// cout << "similarity " << (*it)[0] << " : " << query_word << " = " << similarity << endl;
		// cout << "lengths " << word_ngrams.size() << " " << query_ngrams.size() << endl;
		// cout << "found " << word << endl;
		if (similarity >= similarity_threshold) {
			// Replace the term with highlight
			// cout << "replacing " << (*it)[0] << " with " << highlight_word << endl;
			replace(data, word, highlight_word);
			found = true;
		}
	}
	return found;
}

int main(int argc, char **argv) {
	if (argc < 7) {
        cout << "ERROR: invalid input parameters!" << endl;
        cout << "bio_search <index_name> <top-k> <n-gram length> <n-gram unit length> <similarity threshold> <keyword1> <keyword2> ..." << endl;
        exit(1);
    }
    char* index_name = (char*)argv[1];
    int k = atoi((argv[2]));
    int ngram_length = atoi(argv[3]);
    int ngrams_unit_length = atoi(argv[4]);
    double similarity_threshold = atof(argv[5]);
    int query_terms_start = 6;
    int num_search_terms = argc - query_terms_start;

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
	    // Construct the query terms and query string
	    vector<string> query_terms;
	    string query_string;
	    for (int i = query_terms_start; i < argc; ++i) {
	    	char* term = argv[i];
			// transform the search terms to lower case
			char c; int j = 0;
			while (term[j]) {
				term[j] = tolower(term[j]);
				j++;
			}
			set<string> ngrams;
	    	if (term[0] == '+') {
	    		query_terms.push_back(term+1);
	    		// Ngramize the term and add them to query string
	    		string term_str(term+1);
	    		ngrams = ngram_tokenizer(term_str, ngram_length, ngrams_unit_length);
	    	} else {
	    		query_terms.push_back(term);
	    		string term_str(term);
	    		ngrams = ngram_tokenizer(term_str, ngram_length, ngrams_unit_length);
	    	}
	    	for (set<string>::iterator it = ngrams.begin(); it != ngrams.end(); ++it) {
				if (term[0] == '+') {
					query_string.append("+").append(*it).append(" ");
				} else {
					query_string.append(*it).append(" ");
				}
			}
	    }

	    Xapian::QueryParser parser;
	    Xapian::Query query = parser.parse_query(query_string);

	    // Pass in operator and vector iterators
		// Xapian::Query query(Xapian::Query::OP_OR, query_terms.begin(), query_terms.end());
		cout << "Performing query `" << query.get_description() << "`" << endl;

		// Construct the match decider that uses jaccard distance measure
		JaccardMatchDecider jaccardMatchDecider(similarity_threshold);
		// Add the query ngrams one by one
		for (int j = 0; j < num_search_terms; j++) {
			set<string> tokens = ngram_tokenizer(query_terms.at(j), ngram_length, ngrams_unit_length);
			for (set<string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
				jaccardMatchDecider.add_term(*it);
			}
		}

		// Open the database
	    Xapian::Database db(index_name);

		// Run the query
		Xapian::Enquire enquire(db);
		enquire.set_query(query);

		// Search
		Xapian::MSet matches = enquire.get_mset(0, k, 0, 0, &jaccardMatchDecider);
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
			//cout << data << endl;
			for (int j = 0; j < num_search_terms; j++) {
				// Highlight the fuzzy term in this data
				// cout << "highlighting " << query_terms.at(j) << endl;
				fuzzy_highlight(data, query_terms.at(j), ngram_length, 
					ngrams_unit_length, similarity_threshold);
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
