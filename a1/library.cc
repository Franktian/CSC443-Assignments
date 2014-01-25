#include "library.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cassert>

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
    memcpy((void*)record, buf, size);
    
    return;
}

/* ########## Part 2: Page Layout ########## */

/**
 * Helper Function - Returns how many bytes need for the slot directory
 * indicators.
 */
int fixed_indicator_bit_array_length(int numSlots) {
    return numSlots / 8 + 1;
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
    memset(page->data, '\0', page_size);
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
    
    
}

/**
 * Helper Function - Find a free slot in the page
 * Returns:
 *    record slot offset if successful
 *    -1 if page is full (unsuccessful)
 */
int findAvailableSlot(Page *page) {
    
    return -1;
}

/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 *
 */
int add_fixed_len_page(Page *page, Record *r) {
    
    
    
    
    return 0;
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
    memcpy((void*)data,(void*)r, page->slot_size);
    
    // Update the slot directory indicator bit
    // FIXME
    setSlotOccupied(page, slot);
    
    return;
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
    memcpy((void*)r, (void*)data, page->slot_size);
    
    return;
}

/* Heapfile functions */

/** 
 * Write a page to a file, at the position given by offset 
 * set start_begin to false to start the offset at the end of the file
 */
void _write_page_to_file(Page* page, Offset offset, FILE* file, bool start_begin = true) {
    fseek(file, offset, start_begin ? SEEK_SET : SEEK_END);
    fwrite(page->data, page->page_size, 1, file);
    fflush(file);
}

/** 
 * Read a page from a file, at the position given by offset
 */
void _read_page_from_file(Page* page, Offset offset, FILE* file) {
    fseek(file, offset, SEEK_SET);
    fread(page->data, page->page_size, 1, file);
}

/**
 * Get the position at the end of the file 
 */
Offset _get_eof_offset(FILE* file) {
    fseek(file, 0, SEEK_END);
    return ftell(file);
}

/** 
 * Free a page of its data
 */
void _free_page(Page* page) {
    delete[] (char*)(page->data);
    delete page;
}

// Return the size of the serialized directory page record in bytes
inline int _calc_directory_page_slot_size() {
    return sizeof(Offset)*3;
}

/**
 * Initialize a fixed length record with 3 attributes
 */
void _init_fixed_len_record_int_attr(Record* record, Offset attr1, Offset attr2, Offset attr3) {
    char* bytes_buf = new char[sizeof(Offset)];
    memcpy(bytes_buf, &attr1, sizeof(Offset));
    record->push_back(bytes_buf);
    memcpy(bytes_buf, &attr2, sizeof(Offset));
    record->push_back(bytes_buf);
    memcpy(bytes_buf, &attr3, sizeof(Offset));
    record->push_back(bytes_buf);
    delete[] bytes_buf;
}

/**
 * Initialize a directory header record (just another Record type)
 * A drectory header record has 3 attributes:
 * 0: byte offset of this directory
 * 1: byte offset of the next directory
 * 2: A signature number used for validation after parsing
 */
void _init_directory_header(Record* record, Offset offset) {
    assert(offset >= 0);
    // When initialized, the header has next directory pointer
    // pointing to the first directory
    _init_fixed_len_record_int_attr(record, offset, FIRST_DIRECTORY_OFFSET, DIRECTORY_RECORD_SIGNATURE);
}

/**
 * Initialize a directory record
 * A drectory record has 3 attributes:
 * 0: byte offset of the page pointed by this record
 * 1: number of free slots in the page
 * 2: A signature number used for validation after parsing
 */
void _init_directory_record(Record* record, Offset page_offset, int free_slots) {
    assert(page_offset >= 0 && free_slots >= 0);
    _init_fixed_len_record_int_attr(record, page_offset, free_slots, DIRECTORY_RECORD_SIGNATURE);
}

/* Read a value from the directory record, given index of the value */
Offset _read_directory_record(Record* record, int index) {
    assert(index <= 3 && index >= 0);
    Offset value;
    memcpy(&value, (char*) record->at(0), sizeof(Offset));
    return value;
}

/* Set the directory record with some value */
void _write_directory_record(Record* record, int index, Offset value) {
    assert(index <= 3 && index >= 0);
    memcpy((char*)record->at(0), &value, sizeof(Offset));
}

/**
 * Initialize a directory page and create the header record
 * A directory page is used for pointing to data pages
 * The page has to be initialized with init_fixed_len_page before
 */
void _init_directory_page(Page* page, Offset offset) {
    Record* header = new Record;
    _init_directory_header(header, offset);
    int ret = add_fixed_len_page(page, header);
    assert(ret != -1);
}

/* Read the last directory page and its header of the heapfile */
void _read_last_directory(Page* directory_page, Record* header, Heapfile* heapfile) {
    Offset last_directory_offset;
    do {
        init_fixed_len_page(directory_page, heapfile->page_size, _calc_directory_page_slot_size());
        _read_page_from_file(directory_page, FIRST_DIRECTORY_OFFSET, heapfile->file_ptr);
        read_fixed_len_page(directory_page, 0, header);
        // Read the 2nd entry of the header
        last_directory_offset = _read_directory_record(header, 1);
    } while (last_directory_offset != FIRST_DIRECTORY_OFFSET);
}

void init_heapfile(Heapfile *heapfile, int page_size, FILE *file) {
    heapfile->file_ptr = file;
    heapfile->page_size = page_size;

    // Check if this is a new heapfile by checking if it has a directory
    // page at the beginning of the file with a valid directory header,
    // which contains the signature number. 
    Page* directory_page = new Page();
    init_fixed_len_page(directory_page, page_size, _calc_directory_page_slot_size());
    _read_page_from_file(directory_page, FIRST_DIRECTORY_OFFSET, file);
    
    // Read the header record at the first slot of the page
    Record* header = new Record();
    read_fixed_len_page(directory_page, 0, header);

    // If the header was not correctly parsed, then it is a new heapfile
    if (! _read_directory_record(header, 0) == FIRST_DIRECTORY_OFFSET || 
        ! _read_directory_record(header, 2) == DIRECTORY_RECORD_SIGNATURE) {
        // Create the first directory page
        _init_directory_page(directory_page, FIRST_DIRECTORY_OFFSET);

        // Write the page to the file
        _write_page_to_file(directory_page, FIRST_DIRECTORY_OFFSET, file);
    }

    // Free the directory page being used for checking
    _free_page(directory_page);
}

PageID alloc_page(Heapfile *heapfile) {
    // Initialize a fixed length page
    Page* page = new Page();
    init_fixed_len_page(page, heapfile->page_size, 1000); // TO-DO: extract this

    // Find the last directory page in the heapfile
    Page* directory_page = new Page();
    Record* header = new Record();
    _read_last_directory(directory_page, header, heapfile);
    Offset directory_offset = _read_directory_record(header, 0);

    // We don't need to check if this directory is full here,
    // because we will create a new directory whenever the one we
    // are using is full

    // Create a page record to store in this directory
    Record* page_record = new Record();
    // A new page is always added to the end of the heapfile.
    int page_offset = _get_eof_offset(heapfile->file_ptr);
    _init_directory_record(page_record, page_offset, page->capacity);

    // Write the page to heapfile
    _write_page_to_file(page, page_offset, heapfile->file_ptr);

    // Add the page record to the directory page
    int ret = add_fixed_len_page(directory_page, page_record);
    assert(ret != -1);

    // Check if the current directory is full
    int num_free_slots = fixed_len_page_freeslots(directory_page);
    if (num_free_slots == 0) {
        // If full, we need to create a new directory
        Page* new_directory_page = new Page();
        // Create a fixed length page first
        init_fixed_len_page(new_directory_page, heapfile->page_size, _calc_directory_page_slot_size());
        // Initialize this page as a directory page (adding header)
        int new_directory_offset = _get_eof_offset(heapfile->file_ptr);
        _init_directory_page(new_directory_page, new_directory_offset);
        // Write the new directory page to file
        _write_page_to_file(new_directory_page, new_directory_offset, heapfile->file_ptr);

        // Update the header of the previous directory
        _write_directory_record(header, 0, new_directory_offset);
        write_fixed_len_page(directory_page, 0, header);

        // Free the new directory page used
        _free_page(new_directory_page);
    }

    // Write the directory page to file
    _write_page_to_file(directory_page, directory_offset, heapfile->file_ptr);

    // Free resource used
    _free_page(page);
    _free_page(directory_page);

    // We use the new page's offset as the ID of the page
    return page_offset;
}

void read_page(Heapfile *heapfile, PageID pid, Page *page) {
    // Since Page id is the offset of the page in the heapfile
    _read_page_from_file(page, pid, heapfile->file_ptr);
}

void write_page(Page *page, Heapfile *heapfile, PageID pid) {
    _write_page_to_file(page, pid, heapfile->file_ptr);
}


/**
 * DirectoryIterator constructor
 */
DirectoryIterator::DirectoryIterator(Heapfile* heapf) {
    heapfile = heapf;
}


DirectoryIterator::~DirectoryIterator() {

}

bool DirectoryIterator::hasNext() {
    return false;
}

Page* DirectoryIterator::next() {
    return new Page();
}


 /**
  * PageIterator constructor
  */
PageIterator::PageIterator(Heapfile* heapf, Page* directory) {
    heapfile = heapf;
}
PageIterator::~PageIterator() {

}
bool PageIterator::hasNext() {
    return false;
}
Page* PageIterator::next() {
    return new Page();
}



/**
 * RecordIterator constructor
 */
 RecordIterator::RecordIterator(Heapfile *heapf) {
    heapfile = heapf;
 }
 RecordIterator::~RecordIterator() {

 }
/**
 * Get the next non-empty record in the heap
 */
Record RecordIterator::next() {
    return *(new Record());
}

/**
 * Check if the heap has anymore non-empty record
 */
bool RecordIterator::hasNext() {
    return false;
}