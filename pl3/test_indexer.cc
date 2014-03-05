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
		doc.add_term("people");
		doc.add_term("live");
		doc.add_term("glass");
		doc.add_term("houses");
		doc.add_term("stones");
		doc.add_value(0, string("First document"));
		doc.set_data("people who live in glass houses should not throw stones");
		db.add_document(doc);

		// Do commit in batch
		db.commit();

		// Clear the document ready for the next
		doc.clear_terms();
		doc.clear_values();

		doc.add_term("look");
		doc.add_term("gift");
		doc.add_term("horse");
		doc.add_term("mouth");
		doc.add_value(0, string("Second document"));
		doc.set_data("Don't look a gift horse in the mouth");
		db.add_document(doc);

		db.commit();
	} catch (const Xapian::Error & error) {
		cout << "Exception: " << error.get_msg() << endl;
	}
	
	return 0;
}


