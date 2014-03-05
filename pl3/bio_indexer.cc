#include <xapian.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
 
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
		while (getline(file, line)) {
			cout << line << endl;
			cout << "******" << endl;
		}
	}

	

	file.close();

	return 0;

}