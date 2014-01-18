#include "library.h"
#include <iostream>

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record) {
    int numAttr = record->size();
    int length = 0;
    for (int i=0; i<numAttr; i++) {
        length += sizeof(record[i]);
    }
    return length;
}