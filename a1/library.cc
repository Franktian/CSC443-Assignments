#include "library.h"
#include <iostream>
#include <cstring>

using namespace std;

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record) {
    int numAttr = record->size();
    int length = 0;
    for (int i=0; i<numAttr; i++) {
        length += strlen(record->at(i));
    }
    return length;
}