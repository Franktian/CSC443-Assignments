#include "library.h"
#include <iostream>
#include <cstring>
#include <assert.h>

using namespace std;

/* ########## Part 1: Record Serialization ########## */

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record) {
    
    // Record pointer is NULL, simply return size of 0.
    if (!record)
        return 0;
    
    // Get the number of attributes for this record
    int numAttr = record->size();
    int length = 0;
    for (int i=0; i<numAttr; i++) {
        // Find the character length of each record
        length += strlen(record->at(i));
    }
    // Return the total length
    return length;
}

/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf) {

    // Record pointer or Buffer pointer is NULL - Do Nothing.
    if (!record || !buf)
        return;
    
    char *position = (char*)buf;
    // Copy the record into the buf
    int numAttr = record->size();
    for (int i=0; i<numAttr; i++) {
        // Buffer is full, return.
        if (!position)
            return;
        // Iterate over the all the attributes and copy them into the buf
        strncpy (position, record->at(i), strlen(record->at(i)));
        position += strlen(record->at(i));
    }
    return;
}


/**
 * Deserializes from `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record) {
    
    if (!buf) {
        cout << "ERROR fixed_len_read(): buf is NULL" << endl;
        return;
    }
    
    // Copy the value into the record
    assert(size == fixed_len_sizeof(record));
    for (int i = 0; i < record->size(); i++) {
        // SCHEMA_ATTRIBUTE_LEN is fix = 10 bytes
        memcpy((char*)record, (char*)buf + SCHEMA_ATTRIBUTE_LEN * i, SCHEMA_ATTRIBUTE_LEN);
    }
    return;
}

/* ########## Part 2: Page Layout ########## */

/**
 * Helper Function - Returns how many bytes need for the slot directory
 * indicators.
 */
int fixed_indicator_bit_array_length(int numSlots) {
    return numSlots % 8 == 0 ? numSlots/8 : numSlots/8 + 1;
}

/**
 * Calculates the maximal number of records that fit in a page
 * Overheads: Last 4 bytes indicates the # of slots and a couple of bytes
 *            used for the slot directory indicators. (Each indicator is 1 bit)
 */
int fixed_len_page_capacity(Page *page) {

    if (!page) {
        cout << "ERROR fixed_len_page_capacity(): Page pointer is NULL!" << endl;
        return 0;
    }
    
    int numSlots = page->page_size / page->slot_size;
    int overhead = page->page_size % page->slot_size;
    
    // Find the exact number of slots that allows enough space
    // for the overhead
    while (overhead < sizeof(int) + fixed_indicator_bit_array_length(numSlots)) {
        numSlots--;
        overhead += page->slot_size;
    }
    
    return numSlots;
}

/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size) {
    
    // Assign th page_size and slot_size
    page->page_size = page_size;
    page->slot_size = slot_size;
    page->data = new char[page_size];
    
    // initialize the tail, which contains the # of total slots
    memset(page->data, 0, page_size);
    // Advance the pointer to point the last 4 bytes of the data buffer.
    char *position = (char*)page->data + page_size - sizeof(int); // The last 4 bytes (int) has the # of total slots
    int capacity = fixed_len_page_capacity(page);
    *(int*)position = capacity;
}

/**
 * Helper Function: Calculate how many bits are 1s in a unsigned short
 */
int count1s(unsigned char x) {
    
    short sum = x;
    while (x!=0) {
        x = x >> 1;
        sum = sum - x;
    }
    return (int)sum;
}

/**
 * Calculate the free space (number of free slots) in the page
 * Approach: Calculate the occupied slots and return the difference
 */
int fixed_len_page_freeslots(Page *page) {
    
    if (!page || !page->data)
        return 0;

    int occupiedSlots = 0;
    // Index of start of the bytes that represents
    // the total number of slots
    int index = page->page_size - sizeof(int);
    
    // Calculate the number of occupied slots.
    int numSlots = *(int*)((char*)(page->data) + index);
    int numSlotIndicators = fixed_indicator_bit_array_length(numSlots);
    int i = 0;
    while (i < numSlotIndicators) {
        index--;
        char *data = ((char*)page->data + index);
        occupiedSlots += count1s(*(char*)data);
        i++;
    }

    // Return the number of free slots
    return numSlots - occupiedSlots;
}

/**
 * Helper Function - Set corresponding slot directory indicator bit to 1
 */
void setSlotOccupied(Page *page, int slot) {
    // Find the appropriate byte
    int byte = (slot-1) / 8; 

    // Move to the byte where stores the according slot directory indicator
    char *directory = (char*)page->data + page->page_size - 1 - byte; 
   
    // Generate the masking bit
    int offset = (slot-1) % 8;
    char bit = 0x00;
    int i = 0;
    for (i=0; i<offset; i++) {
       if (i == 0)
           bit = bit | 0x1;
       else
           bit = bit << 1;  
    } 

    // Mask the corresponding bit to 1
    *directory = *directory | bit;
    return; 
}

/**
 * Helper Function - Find a free slot in the page
 * Returns:
 *    record slot offset if successful
 *    -1 if page is full (unsuccessful)
 */
int findAvailableSlot(Page *page) {
    int slotIndex = 0;
    // Move to beginning of the slot directory
    char *directory = (char*)page->data + page->page_size - 1; 
    int  pageCapacity =  fixed_len_page_capacity(page);

    // Iterate each byte for the slot directory and search to 0 bit
    int i = 0;
    char byte = *directory;
    while (i < pageCapacity/8) {
        byte = *directory;
        int j = 0;
        int value = 0;
        while (j < 7) {
            // Extract the last bit, check whether it is 0.
            value = byte & 0x1;
            if (value == 0)
                return i*8 + j; // Corresponding slot index
            // Since the bit is 1, shift to the right by 1 bit.
            else
                byte = byte >> 1;
            j++;
        }
        i++;
        directory--;
    }

    // Extra Process of the left over bits.
    // Find out how many extra bits need to be processed
    int extra = pageCapacity % 8;
    i = 0;
    byte = *directory;
    while (i < extra) {
 
        int value = 0;
        value = byte & 0x1;
        if (value == 0) {
            int tempValue = pageCapacity/8;
            return tempValue*8 + i;
        }
        else
            byte = byte >> 1;

        i++;
    }

    // No available slots find, return -1
    return -1;    
}

/**
 * Write a record into a given slot.
 * Assumption: Slot # start with 0
 */
void write_fixed_len_page(Page *page, int slot, Record *r) {
    // Check if Record is NULL or the page has not been initialized
    if (!page || !page->data || !r) {
        cout << "ERROR write_fixed_len_page(): NULL pointers passed in!" << endl;
        return;
    }   
       
    int recordSize = fixed_len_sizeof(r);
    int numSlots = *(int*)((char*)(page->data) + page->page_size - sizeof(int));
    // Slot input is invalid
    if (slot+1 > numSlots || slot < 0) {
        cout << "ERROR write_fixed_len_page(): Invalid slot number!" << endl;
        return;
    }   
       
    // Find the location to write
    char *data = (char*)page->data + page->slot_size*slot;
    // Write the record into the given slot
    fixed_len_write(r, (void*) data);   
    // Update the slot directory indicator bit
    setSlotOccupied(page, slot);
    
    return;
}

/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 *
 */
int add_fixed_len_page(Page *page, Record *r) {
    
   if (!page || !page->data || !r) {
       cout << "ERROR add_fixed_len_page(): Invalid inputs - NULL Pointers!" << endl;
       return -1; 
   }    

   // Find available slot for the record    
   int slot = findAvailableSlot(page);
   if ( slot == -1 ) {
       // Page is full
       cout << "Warning: Page is full, unable to add a record to page!" << endl;
       return -1;
   }

   // Find slot position and copy the data.
   write_fixed_len_page(page, slot, r);

   return 0; 
}

/**
 * Read a record from the page from a given slot.
 * Assumption: Slot # start with 0
 */
void read_fixed_len_page(Page *page, int slot, Record *r) {
    
    // Check if Record is NULL or the page has not been initialized
    if (!page || !page->data || !r) {
        cout << "ERROR read_fixed_len_page(): NULL pointers passed in!" << endl;
        return;
    }
    
    int recordSize = fixed_len_sizeof(r);
    int numSlots = *(int*)((char*)(page->data) + page->page_size - sizeof(int));
    // Slot input is invalid
    if (slot+1 > numSlots || slot < 0) {
        cout << "ERROR read_fixed_len_page(): Invalid slot number!" << endl;
        return;
    }
    
    // Find the location to read
    char *data = (char*)page->data + page->slot_size*slot;
    fixed_len_read((void*)data, fixed_len_sizeof(r), r);
    
    return;
}
