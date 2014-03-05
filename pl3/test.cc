#include <xapian.h>
#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

int main(int argc, char **argv) {
	char dbname[] = "...";
	try {
		Xapian::WritableDatabase db(dbname, Xapian::DB_CREATE_OR_OPEN);
		Xapian::Document doc;
		std::string term = "hello";
		doc.add_term(term);
		doc.add_value(0, string("hello"));
		doc.add_value(1, string("world"));
		db.add_document(doc);

		// Do commit in batch
		db.commit();

		Xapian::Database qdb(dbname);
		std::vector<string> terms;
		terms.push_back("hello");
		terms.push_back("world");

		// Pass in operator and vector iterators
		Xapian::Query query(Xapian::Query::OP_OR, terms.begin(), terms.end());
		cout << "Performing query `" << query.get_description() << "`" << endl;

		//Start query
		Xapian::Enquire enquire(db);
		enquire.set_query(query);

		Xapian::MSet matches = enquire.get_mset(0, 10);
		cout << "mset size is " << matches.size() << endl;

	} catch (const Xapian::Error & error) {
		cout << "Exception: " << error.get_msg() << endl;
	}
	
	return 0;
}


