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
    page->data = new char[page_size];
    
    // initialize the head
    memset(page->data, '\0', page_size);
    char *position = (char*)page->data + page_size - sizeof(int);
    int capacity = fixed_len_page_capacity(page);
    *(int*)position = capacity;
}

int fixed_indicator_bit_array_length(int numSlot) {
    return numSlot / 8 + 1;
}

/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page) {
    int numrec = page->page_size / page->slot_size;
    int overhead = page->page_size % page->slot_size;
    
    // Find the number of record that allows enough space
    // for the overhead
    while (overhead < sizeof(numrec) + fixed_indicator_bit_array_length(numrec)) {
        numrec--;
        overhead += page->slot_size;
    }
    return numrec;
}

int count1(unsigned short x) {
    short sum = x;
    while (x!=0) {
        x = x >> 1;
        sum = sum - x;
    }
    return sum;  
}

int fixed_len_page_freeslots(Page *page) {
    int numFreeSlots = 0;
    int occupiedSlots = 0;
    // Get the index of the start of the bytes that represent
    // the number of records
    int i = page->page_size - sizeof(int);
    // Get the number of total slots
    int numSlot = *(int*)((char*)(page->data) + i);
    int numIndicatorBytes = fixed_indicator_bit_array_length(numSlot);
    int j = 0;
    while (j < numIndicatorBytes) {
        i--;
        char *data = ((char*)page->data + i);
        occupiedSlots += count1(*(short*)data);
        j++;
    }
    numFreeSlots = numSlot - occupiedSlots;
    return numFreeSlots;
}