#include <xapian.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include "common.h"
 
using namespace std;

int main(int argc, char **argv) {

	if (argc < 3) {
		cerr << "Usage: <parsed_biographies> <index_name>" << endl;
        exit(0);
	}
}