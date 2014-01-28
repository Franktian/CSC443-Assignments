#include "library.h"
#include <iostream>
#include <cstring>
#include <assert.h>

using namespace std;

#define LIB_DEBUG 0
/* ########## Part 1: Record Serialization ########## */

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record) {
    
    // Record pointer is NULL, simply return size of 0.
    if (!record) {
        cout << "ERROR fixed_len_sizeof(): Invalid input parameter - Record is null!" << endl;
        return 0;
    }

    // Get the number of attributes for this record
    int numAttr = record->size();
    int length = 0;
    for (int i=0; i<numAttr; i++) {
        // Find the character length of each record
        length += strlen(record->at(i)) + 1;
    }
    // Return the total length
    return length;
}

/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf) {

    // Record pointer or Buffer pointer is NULL - Do Nothing.
    if (!record || !buf) {
        cout << "ERROR fixed_len_write(): Invalid Input parameters!" << endl;
        return;
    }

    char *position = (char*)buf;
    // Copy the record into the buf
    int numAttr = record->size();
    for (int i=0; i<numAttr; i++) {
        // Iterate over the all the attributes and copy them into the buf
        memcpy (position, record->back(), SCHEMA_ATTRIBUTE_LEN);
    	record->pop_back();
    	if (LIB_DEBUG) {
    	    char buffer[1000];
    	    strncpy (buffer, record->at(i), SCHEMA_ATTRIBUTE_LEN);
    	    cout << "###### Serializing Attribute: " << record->at(i) << " ######" << endl;
        }
        position += SCHEMA_ATTRIBUTE_LEN;
    }
    if (LIB_DEBUG) 
    	cout << (char*) buf << endl;

    return;
}


/**
 * Deserializes from `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record) {
    
    if (!buf || !record) {
        cout << "ERROR fixed_len_read(): Invalid input parameter!" << endl;
        return;
    }
    
    // Copy the value into the record
    assert(size == fixed_len_sizeof(record));
    for (int i = 0; i < SCHEMA_ATTRIBUTES_NUM; i++) {
        // SCHEMA_ATTRIBUTE_LEN is fix = 10 bytes
        char *buffer = new char[10];
        strncpy(buffer, (char*)buf + SCHEMA_ATTRIBUTE_LEN * i, SCHEMA_ATTRIBUTE_LEN);
        if (LIB_DEBUG)
	    cout << "Content : " << buffer << endl;
        
        record->at(i) = buffer;
        if (LIB_DEBUG)
            cout << (char*) record->at(i) << endl;
    }
    return;
}

/* ########## Part 2: Page Layout ########## */

/**
 * Helper Function - Returns how many bytes need for the slot directory
 * indicators.
 */
/* Version 1
int fixed_indicator_bit_array_length(int numSlots) {
    return numSlots % 8 == 0 ? numSlots/8 : numSlots/8 + 1;
}
*/

/**
 * Calculates the maximal number of records that fit in a page
 * Overheads: Last 4 bytes indicates the # of slots and a couple of bytes
 *            used for the slot directory indicators. (Each indicator is 1 bit)
 */
/* Version 1
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
*/

// Version 2
int fixed_len_page_capacity(Page *page) {
    // Structure of each slot - 1 byte indicator instead of 1 bit indicator
    return page->page_size/(sizeof(char) + page->slot_size);
}

/**
 * Initializes a page using the given slot size
 */
/* Version 1
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
*/

// Version 2
// Capacity field is added to the Page structure
void init_fixed_len_page(Page *page, int page_size, int slot_size) {
    
    // Assign th page_size and slot_size
    page->page_size = page_size;
    page->slot_size = slot_size;
    page->data = new char[page_size];
    page->capacity = fixed_len_page_capacity(page);

    // initialize the tail, which contains the # of total slots
    memset(page->data, 0, page_size);
}

/**
 * Helper Function: Calculate how many bits are 1s in a unsigned short
 * Version 1 implementation
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
/* Version 1
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
*/

// Version 2
int fixed_len_page_freeslots(Page *page) {
    
    if (!page || !page->data)
        return 0;

    int occupiedSlots = 0;

    // Calculate the number of occupied slots.
    int i = 0;
    char *directory = (char*) page->data;
    while (i < page->capacity) {
        // Increment the occupied slot counter if directory byte is 1
        if ( *directory == 1 ) 
            occupiedSlots++;
        // Advance to next directory indicator byte
        directory++;
        i++;
    }
    // Return the number of free slots
    return page->capacity - occupiedSlots;
}

/**
 * Helper Function - Set corresponding slot directory indicator bit to 1
 */
/* Version 1
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
*/

/**
 * Helper Function - Find a free slot in the page
 * Returns:
 *    record slot offset if successful
 *    -1 if page is full (unsuccessful)
 */
/* Version 1
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
*/

// Version 2
int findAvailableSlot(Page *page) {
    char *directory = (char*) page->data;
    int slot = 0;
    // Iterate over the directory to find the first non-occupied slot
    for (int i = 0; i < page->capacity; i++) {
       	if (directory[i] != 1) {
    	    slot = i; // Find the available slot
    	    break;
        }     
    }
    return slot;
}

/**
 * Write a record into a given slot.
 * Assumption: Slot # start with 0
 */
/* Version 1
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
*/

// Version 2
void write_fixed_len_page(Page *page, int slot, Record *r) {
    // Check if Record is NULL or the page has not been initialized
    if (!page || !page->data || !r) {
        cout << "ERROR write_fixed_len_page(): NULL pointers passed in!" << endl;
        return;
    }

    int numSlots = page->capacity;
    // Slot input is invalid
    if (slot >= numSlots || slot < 0) {
        cout << "ERROR write_fixed_len_page(): Invalid slot number!" << endl;
        return;
    }
       
    // Find the location to write
    char *data = (char*)page->data + page->capacity + page->slot_size*slot;
    // Write the record into the given slot
    cout << "WRITE_PAGE() ****** " << r->at(0) << " &&&&&& " << r->at(1) << " ******" << endl; 
    fixed_len_write(r, (void*) data);   
    // Update the slot directory indicator byte
    ((char*)page->data)[slot] = 1;
    
    return;
}

/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 *
 */
/* Version 1
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
*/

// Version 2
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
   cout << "(ADD_PAGE)****** " << r->at(0) << " &&&&&& " << r->at(60) << " ******" << endl;
   write_fixed_len_page(page, slot, r);
   return slot; 
}

/**
 * Read a record from the page from a given slot.
 * Assumption: Slot # start with 0
 */
/* Version 1
void read_fixed_len_page(Page *page, int slot, Record *r) {
    
    // Check if Record is NULL or the page has not been initialized
    if (!page || !page->data || !r) {
        cout << "ERROR read_fixed_len_page(): NULL pointers passed in!" << endl;
        return;
    }
    
    int numSlots = *(int*)((char*)(page->data) + page->page_size - sizeof(int));
    cout << "NUM OF SLOTS: " << numSlots << " " << page->slot_size << endl;
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
*/

// Version 2
bool read_fixed_len_page(Page *page, int slot, Record *r) {
    
    // Check if Record is NULL or the page has not been initialized
    if (!page || !page->data || !r) {
        cout << "ERROR read_fixed_len_page(): NULL pointers passed in!" << endl;
        exit(1);
    }
    
    int numSlots = page->capacity;
    cout << page->capacity << endl;
    if (LIB_DEBUG) 
        cout << "NUM OF SLOTS: " << numSlots << " " << page->slot_size << endl;
    // Slot input is invalid
    if (slot+1 > numSlots || slot < 0) {
        cout << "ERROR read_fixed_len_page(): Invalid slot number!" << endl;
        exit(1);
    }
    // Slot is empty
    if (((char*)page->data)[slot] != 1) {
    	cout << "ERROR read_fixed_len_page(): slot position is empty!" << endl;
    	return false;
    }

    // Find the location to read
    char *data = (char*)page->data + page->capacity + page->slot_size*slot;
    fixed_len_read((void*)data, fixed_len_sizeof(r), r);

    return true;
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
    return sizeof(Offset) * 3;
}

inline int _calc_directory_page_capacity(int page_size) {
    return page_size / _calc_directory_page_slot_size();
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
    Record* header = new Record();
    _init_directory_header(header, offset);
    int ret = add_fixed_len_page(page, header);
    assert(ret != -1);
}

/**
 * Locate the directory given the page offset
 * The directory page must be initialized before passed in
 * Return the directory offset of the located directory
 */
Offset _locate_directory(Page* directory_page, Record* header, Offset page_offset, Heapfile* heapfile) {
    Offset directory_offset = FIRST_DIRECTORY_OFFSET;
    Offset curr_directory_offset;
    do {
        _read_page_from_file(directory_page, directory_offset, heapfile->file_ptr);
        read_fixed_len_page(directory_page, 0, header);
        curr_directory_offset = directory_offset;
        // Get the offset of the next directory
        directory_offset = _read_directory_record(header, 1);
        // If the offset of the next directory is greater than the page offset
        // we have found the directory of the page
        if (directory_offset > page_offset) {
            break;
        }
        // Else we will keep looking
    } while (directory_offset != FIRST_DIRECTORY_OFFSET);
    // The loop terminates at the last directory (or the only directory),
    // where the page would belong to.
    return curr_directory_offset;
}

/**
 * Read the last directory page and its header of the heapfile
 * The directory page must be initialized before passed in.
 * Return the slot number of the page record in the directory
 */
void _read_last_directory(Page* directory_page, Record* header, Heapfile* heapfile) {
    Offset eof = _get_eof_offset(heapfile->file_ptr);
    _locate_directory(directory_page, header, eof, heapfile);
}

/* Locate and read the page record in a directory given the page ID */
int _read_page_record(Page* directory_page, Record* page_record, PageID pid, Heapfile* heapfile) {
    // The directory page must be initialized before passed in

    // Loop through the directory to find our page record
    int directory_capacity = fixed_len_page_capacity(directory_page);
    int curr_slot = 1; // 0 is the header, just ignore
    Offset curr_page_offset;
    do {

        read_fixed_len_page(directory_page, curr_slot, page_record);
        curr_page_offset = _read_directory_record(page_record, 0);
        if (curr_page_offset == pid) {
            break;
        }
        curr_slot++;
    } while (curr_slot < directory_capacity);
    
    // Return the slot number of the page record
    return curr_slot;
}

/* Get the offset of a page given its page ID */
Offset _locate_page(PageID pid, Heapfile *heapfile) {
    Record* page_record = new Record();
    Record* header = new Record();
    Page* directory_page = new Page();

    // First we need to find the directory that contains the page with pid
    init_fixed_len_page(directory_page, heapfile->page_size, _calc_directory_page_slot_size());
    _locate_directory(directory_page, header, pid, heapfile);

    // Get the page record from directory
    _read_page_record(directory_page, page_record, pid, heapfile);

    // Get the offset from the page record in the directory
    Offset page_offset = _read_directory_record(page_record, 0);

    // Just to make sure our current model is working
    assert(page_offset == pid);

    _free_page(directory_page);

    return page_offset;
}

void init_heapfile(Heapfile *heapfile, int page_size, FILE *file) {
    heapfile->file_ptr = file;
    heapfile->page_size = page_size;

    // Get stuff we need to use
    Page* directory_page = new Page();
    Record* header = new Record();

    // Check if this is a new heapfile by checking if it has a directory
    // page at the beginning of the file with a valid directory header,
    // which contains the signature number. 
    init_fixed_len_page(directory_page, page_size, _calc_directory_page_slot_size());
    _read_page_from_file(directory_page, FIRST_DIRECTORY_OFFSET, file);
    
    // Read the header record at the first slot of the page
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

// Parse the first directory from a heapfile
void _locate_first_directory(Heapfile *heapfile, Page *directory, Record *header) {
    _locate_directory(directory, header, FIRST_DIRECTORY_OFFSET, heapfile);
}

PageID alloc_page(Heapfile *heapfile) {
    // Initialize a fixed length page
    Page* page = new Page();
    init_fixed_len_page(page, heapfile->page_size, 1000); // TO-DO: extract this

    // Find the last directory page in the heapfile
    Page* directory_page = new Page();
    Record* header = new Record();
    init_fixed_len_page(directory_page, heapfile->page_size, _calc_directory_page_slot_size());
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
    // Here we are not assuming the page ID is the same as 
    // the page offset, so we need to locate the page record
    // first, and get the offset from there

    // If we change the definition of PageID in the future,
    // we just need to change the logic in _locate_page
    
    Offset page_offset = _locate_page(pid, heapfile);

    // Read the page at the offset position
    _read_page_from_file(page, page_offset, heapfile->file_ptr);
}

void write_page(Page *page, Heapfile *heapfile, PageID pid) {
    Record* page_record = new Record();
    Record* header = new Record();
    Page* directory_page = new Page();

    // First we need to find the directory that contains the page with pid
    init_fixed_len_page(directory_page, heapfile->page_size, _calc_directory_page_slot_size());
    Offset directory_offset = _locate_directory(directory_page, header, pid, heapfile);

    // Get the page record from directory
    int page_record_slot = _read_page_record(directory_page, page_record, pid, heapfile);

    // Get the offset from the page record in the directory
    Offset page_offset = _read_directory_record(page_record, 0);

    // Just to make sure our current model is working
    assert(page_offset == pid);

    // Write the page at the offset position
    _write_page_to_file(page, page_offset, heapfile->file_ptr);

    // Update the page record
    int num_free_slots = fixed_len_page_freeslots(page);
    _write_directory_record(page_record, 1, num_free_slots);
    write_fixed_len_page(directory_page, page_record_slot, page_record);

    // Update the directory page
    _write_page_to_file(directory_page, directory_offset, heapfile->file_ptr);

    // Free
    _free_page(directory_page);
}


/**
 * DirectoryIterator constructor
 */
DirectoryIterator::DirectoryIterator(Heapfile* heapfile) {
    this->heapfile = heapfile;
    this->directory = new Page();
    this->header = new Record();
    init_fixed_len_page(this->directory, this->heapfile->page_size, _calc_directory_page_slot_size());
    _locate_first_directory(this->heapfile, this->directory, this->header);
    this->current_offset = FIRST_DIRECTORY_OFFSET;
}


DirectoryIterator::~DirectoryIterator() {
    _free_page(this->directory);
}

bool DirectoryIterator::hasNext() {
    this->next_offset = _read_directory_record(header, 1);
    return this->next_offset > this->current_offset;
}

Page* DirectoryIterator::next() {
    assert(this->hasNext());
    _read_page_from_file(this->directory, this->next_offset, this->heapfile->file_ptr);
    this->current_offset = this->next_offset;
    return this->directory;
}

 /**
  * PageIterator constructor
  */
PageIterator::PageIterator(Heapfile* heapfile, Page* directory) {
    this->heapfile = heapfile;
    this->directory = directory;
    this->current_page = new Page();
    
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
