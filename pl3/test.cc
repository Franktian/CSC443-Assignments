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
	} catch (const Xapian::Error & error) {
		cout << "Exception: " << error.get_msg() << endl;
	}
	
	return 0;
}


