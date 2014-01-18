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

/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size) {
    page->page_size = page_size;
    page->slot_size = slot_size;
    // initialize the data
}

/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page) {
    int numrec = page->page_size / page->slot_size;
    int overhead = page->page_size % page->slot_size;
    
    // Find the number of record that allows enough space
    // for the overhead
    while (overhead < sizeof(numrec) + numrec/8 + 1) {
        numrec--;
        overhead += page->slot_size;
    }
    return numrec;
}