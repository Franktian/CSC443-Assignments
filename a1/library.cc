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
void fixed_len_write(Record *record, void *buf, int attrLen = SCHEMA_ATTRIBUTE_LEN) {

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
        memcpy (position, record->back(), attrLen);
    	record->pop_back();
    	if (LIB_DEBUG) {
    	    char buffer[attrLen*numAttr];
    	    strncpy (buffer, record->at(i), attrLen);
    	    cout << "###### Serializing Attribute: " << record->at(i) << " ######" << endl;
        }
        position += attrLen;
    }
    if (LIB_DEBUG) 
    	cout << (char*) buf << endl;

    return;
}


/**
 * Deserializes from `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record, 
    int attrLen = SCHEMA_ATTRIBUTE_LEN, int numAttr = SCHEMA_ATTRIBUTES_NUM) {
    
    if (!buf || !record) {
        cout << "ERROR fixed_len_read(): Invalid input parameter!" << endl;
        return;
    }
    
    // Copy the value into the record
    assert(size == fixed_len_sizeof(record));
    for (int i = 0; i < numAttr; i++) {
        // SCHEMA_ATTRIBUTE_LEN is fix = 10 bytes
        char *buffer = new char[attrLen];
        strncpy(buffer, (char*)buf + attrLen * i, attrLen);
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
void write_fixed_len_page(Page *page, int slot, Record *r, int attrLen = SCHEMA_ATTRIBUTE_LEN) {
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
    // cout << "WRITE_PAGE() ****** " << r->at(0) << " &&&&&& " << r->at(1) << " ******" << endl; 
    fixed_len_write(r, (void*) data, attrLen);
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
int add_fixed_len_page(Page *page, Record *r, int attrLen = SCHEMA_ATTRIBUTE_LEN) {
    
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
   // cout << "(ADD_PAGE)****** " << r->at(0) << " &&&&&& " << r->at(1) << " ******" << endl;
   write_fixed_len_page(page, slot, r, attrLen);
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
bool read_fixed_len_page(Page *page, int slot, Record *r, int attrLen = SCHEMA_ATTRIBUTE_LEN,
                            int numAttr = SCHEMA_ATTRIBUTES_NUM) {
    
    // Check if Record is NULL or the page has not been initialized
    if (!page || !page->data || !r) {
        cout << "ERROR read_fixed_len_page(): NULL pointers passed in!" << endl;
        exit(1);
    }
    
    int numSlots = page->capacity;
    // cout << page->capacity << endl;
    if (LIB_DEBUG) 
        cout << "NUM OF SLOTS: " << numSlots << " " << page->slot_size << endl;
    // Slot input is invalid
    if (slot+1 > numSlots || slot < 0) {
        cout << "ERROR read_fixed_len_page(): Invalid slot number!" << endl;
        exit(1);
    }
    // Slot is empty
    if (((char*)page->data)[slot] != 1) {
    	// cout << "ERROR read_fixed_len_page(): slot position is empty!" << endl;
    	return false;
    }

    // Find the location to read
    char *data = (char*)page->data + page->capacity + page->slot_size*slot;
    fixed_len_read((void*)data, fixed_len_sizeof(r), r, attrLen, numAttr);

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
inline int _directory_record_size() {
    return DIRECTORY_RECORD_ATTRIBUTE_LEN*DIRECTORY_RECORD_ATTRIBUTE_NUM;
}

inline int _directory_page_capacity(int page_size) {
    return page_size / _directory_record_size();
}

void _read_directory_header_from_file(DirectoryHeader* header, Offset offset, FILE* file) {
    fseek(file, offset, SEEK_SET);
    fread(&(header->offset), sizeof(Offset), 1, file);
    fread(&(header->next), sizeof(Offset), 1, file);
    fread(&(header->signature), sizeof(Offset), 1, file);
}

void _write_directory_header_to_file(DirectoryHeader* header, Offset offset, FILE* file) {
    fseek(file, offset, SEEK_SET);
    fwrite(&(header->offset), sizeof(Offset), 1, file);
    fwrite(&(header->next), sizeof(Offset), 1, file);
    fwrite(&(header->signature), sizeof(Offset), 1, file);
    fflush(file);
}

void _read_directory_from_file(char* directory, Offset offset, Heapfile* heapfile) {
    fseek(heapfile->file_ptr, offset, SEEK_SET);
    fread(directory, heapfile->page_size, 1, heapfile->file_ptr);
}

void _write_directory_to_file(char* directory, Offset offset, Heapfile* heapfile) {
    // Write the directory to file
    fseek(heapfile->file_ptr, offset, SEEK_SET);
    fwrite(directory, heapfile->page_size, 1, heapfile->file_ptr);
    fflush(heapfile->file_ptr);
}

void _read_directory_record(DirectoryRecord* record, char* directory, int slot) {
    char* i = directory + slot*_directory_record_size();
    memcpy(&(record->page_offset), i, sizeof(Offset));
    i += sizeof(Offset);
    memcpy(&(record->free_slots), i, sizeof(Offset));
    i += sizeof(Offset);
    memcpy(&(record->signature), i, sizeof(Offset));
}

void _write_directory_record(DirectoryRecord* record, char* directory, int slot) {
    char* i = directory + slot*_directory_record_size();
    memcpy(i, &(record->page_offset), sizeof(Offset));
    i += sizeof(Offset);
    memcpy(i, &(record->free_slots), sizeof(Offset));
    i += sizeof(Offset);
    memcpy(i, &(record->signature), sizeof(Offset));
}

void _init_directory_record(DirectoryRecord* record, Offset page_offset, Offset free_slots) {
    record->page_offset = page_offset;
    record->free_slots = free_slots;
    record->signature = DIRECTORY_RECORD_SIGNATURE;
}

void _init_directory_header(DirectoryHeader* header, Offset offset, Offset next) {
    header->offset = offset;
    header->next = next;
    header->signature = DIRECTORY_RECORD_SIGNATURE;
}

void _init_directory(char* directory, int size, DirectoryHeader* header) {
    // initialized header passed in
    int capacity = _directory_page_capacity(size);
    assert(size > 1);
    // Serialize header first
    char* i = directory;
    memcpy(i, &(header->offset), sizeof(Offset));
    i += sizeof(Offset);
    memcpy(i, &(header->next), sizeof(Offset));
    i += sizeof(Offset);
    memcpy(i, &(header->signature), sizeof(Offset));

    DirectoryRecord* record = new DirectoryRecord();
    _init_directory_record(record, EMPTY_PAGE_OFFSET, 0);
    for (int i = 1; i < capacity; ++i) // 1 is the header, ignore
    {
        _write_directory_record(record, directory, i);
    }
}

/**
 * Locate the directory given the page offset
 * The directory page must be initialized before passed in
 * Return the directory offset of the located directory
 */
Offset _locate_directory(DirectoryHeader* header, Offset page_offset, Heapfile* heapfile) {
    Offset directory_offset = FIRST_DIRECTORY_OFFSET;
    Offset curr_directory_offset;
    do {
        _read_directory_header_from_file(header, directory_offset, heapfile->file_ptr);
        curr_directory_offset = directory_offset;
        // Get the offset of the next directory
        directory_offset = header->next;
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

int _locate_available_slot(DirectoryRecord* record, char* directory, int size) {
    int capacity = _directory_page_capacity(size);
    for (int i = 1; i < capacity; ++i) // 1 is the header
    {
        _read_directory_record(record, directory, i);
        if (record->page_offset == EMPTY_PAGE_OFFSET) {
            return i;
        }
    }
    return -1;
}

int _locate_directory_record(DirectoryRecord* record, Offset pid, char* directory, int size) {
    int capacity = _directory_page_capacity(size);
    for (int i = 1; i < capacity; ++i) // 1 is the header
    {
        _read_directory_record(record, directory, i);
        if (record->page_offset == pid) {
            return i;
        }
    }
    return -1;
}

/**
 * Read the last directory page and its header of the heapfile
 * The directory header must be initialized before passed in.
 * Return the slot number of the page record in the directory
 */
Offset _last_directory(DirectoryHeader* header, Heapfile* heapfile) {
    Offset eof = _get_eof_offset(heapfile->file_ptr);
    return _locate_directory(header, eof, heapfile);
}

Offset _locate_page(PageID pid, Heapfile* heapfile) {
    DirectoryHeader* header = new DirectoryHeader();
    DirectoryRecord* record = new DirectoryRecord();
    char* directory = new char[heapfile->page_size];

    // locate the directory for the page record
    Offset directory_offset = _locate_directory(header, pid, heapfile);

    // read the directory from file
    _read_directory_from_file(directory, directory_offset, heapfile);

    // locate the directory record of this pid
    _locate_directory_record(record, pid, directory, heapfile->page_size);

    delete[] directory;

    return record->page_offset;
}

void init_heapfile(Heapfile *heapfile, int page_size, FILE *file) {
    heapfile->file_ptr = file;
    heapfile->page_size = page_size;

    // Get stuff we need to use
    DirectoryHeader* header = new DirectoryHeader();

    // Check if this is a new heapfile by checking if it has a directory
    // page at the beginning of the file with a valid directory header,
    // which contains the signature number. 
    _read_directory_header_from_file(header, FIRST_DIRECTORY_OFFSET, heapfile->file_ptr);
    
    // Read the header record at the first slot of the page
    // If the header was not correctly parsed, then it is a new heapfile
    if (header->signature != DIRECTORY_RECORD_SIGNATURE) {
        _init_directory_header(header, FIRST_DIRECTORY_OFFSET, FIRST_DIRECTORY_OFFSET);
        char* directory = new char[heapfile->page_size];
        _init_directory(directory, heapfile->page_size, header);
        _write_directory_to_file(directory, FIRST_DIRECTORY_OFFSET, heapfile);
        delete[] directory;
    }

    delete header;
}

// Parse the first directory from a heapfile
void _locate_first_directory(Heapfile *heapfile, Page *directory, Record *header) {
    _locate_directory(directory, header, FIRST_DIRECTORY_OFFSET, heapfile);
}

void _locate_first_directory_page(Heapfile *heapfile, Page *directory, Page *page) {

}

PageID alloc_page(Heapfile *heapfile) {
    // Initialize a fixed length page
    Page* page = new Page();
    init_fixed_len_page(page, heapfile->page_size, RECORD_SIZE); // TO-DO: extract this

    // Find the last directory in the heapfile
    DirectoryHeader* header = new DirectoryHeader();
    Offset directory_offset = _last_directory(header, heapfile);
    char* directory = new char[heapfile->page_size];
    _read_directory_from_file(directory, directory_offset, heapfile);

    // We don't need to check if this directory is full here,
    // because we will create a new directory whenever the one we
    // are using is full

    // Find the next available slot for a page record in directory
    DirectoryRecord* record = new DirectoryRecord();
    int slot = _locate_available_slot(record, directory, heapfile->page_size);
    assert(slot != -1);

    // Create a page record to store in this directory    
    // A new page is always added to the end of the heapfile.
    int page_offset = _get_eof_offset(heapfile->file_ptr);
    _init_directory_record(record, page_offset, page->capacity);

    // Write the page to heapfile
    _write_page_to_file(page, page_offset, heapfile->file_ptr);

    // Add the page record to the directory
    _write_directory_record(record, directory, slot);

    // Write the directory page to file
    _write_directory_to_file(directory, directory_offset, heapfile);

    // Check if the current directory is full
    if (slot == _directory_page_capacity(heapfile->page_size)) {
        // If full, we need to create a new directory
        char* new_directory = new char[heapfile->page_size];
        
        // Create the header 
        DirectoryHeader* new_header = new DirectoryHeader();
        int new_directory_offset = _get_eof_offset(heapfile->file_ptr);
        _init_directory_header(new_header, new_directory_offset, FIRST_DIRECTORY_OFFSET);
        
        // Initialize the directory
        _init_directory(new_directory, heapfile->page_size, new_header);

        // Write the new directory to file
        _write_directory_to_file(new_directory, new_directory_offset, heapfile);

        // Update the header of the previous directory
        header->next = new_directory_offset;
        _write_directory_header_to_file(header, directory_offset, heapfile->file_ptr);

        // Free the new directory page used
        delete[] new_directory;
    }

    // Free resource used
    _free_page(page);
    delete[] directory;

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

    assert(pid == page_offset);

    // Read the page at the offset position
    _read_page_from_file(page, page_offset, heapfile->file_ptr);
}

void write_page(Page *page, Heapfile *heapfile, PageID pid) {
    DirectoryHeader* header = new DirectoryHeader();
    DirectoryRecord* record = new DirectoryRecord();
    char* directory = new char[heapfile->page_size];

    // locate the directory for the page record
    Offset directory_offset = _locate_directory(header, pid, heapfile);

    // read the directory from file
    _read_directory_from_file(directory, directory_offset, heapfile);

    // locate the directory record of this pid
    int slot = _locate_directory_record(record, pid, directory, heapfile->page_size);

    assert(slot != -1);

    int page_offset = record->page_offset;

    // Just to make sure our current model is working
    assert(page_offset == pid);

    // Write the page at the offset position
    _write_page_to_file(page, page_offset, heapfile->file_ptr);

    // Update the page record
    record->free_slots = fixed_len_page_freeslots(page);
    _write_directory_record(record, directory, slot);

    // Update the directory page
    _write_directory_to_file(directory, directory_offset, heapfile);    

    // Free
    delete[] directory;
}


/**
 * DirectoryIterator constructor
 */
DirectoryIterator::DirectoryIterator(Heapfile* heapfile) {
    this->heapfile = heapfile;
    this->directory = new Page();
    this->header = new Record();
    // Initialize the directory page and header record
    init_fixed_len_page(this->directory, this->heapfile->page_size, _calc_directory_page_slot_size());
    _locate_first_directory(this->heapfile, this->directory, this->header);
    // Set the current offset to 0
    this->current_offset = FIRST_DIRECTORY_OFFSET;
}


DirectoryIterator::~DirectoryIterator() {
    _free_page(this->directory);
}

bool DirectoryIterator::hasNext() {
    // Get the next offset with the current header record
    this->next_offset = _read_directory_record(header, 1);
    return this->next_offset > this->current_offset;
}

Page* DirectoryIterator::next() {
    // Confirm we have another directory in the heafile
    assert(this->hasNext());
    // Set the next directory and its header
    _read_page_from_file(this->directory, this->next_offset, this->heapfile->file_ptr);
    // Set the next offset
    this->current_offset = this->next_offset;
    return this->directory;
}

/**
 * PageRecordIterator constructor
 */
 PageRecordIterator::PageRecordIterator(Page *page) {
    this->page = page;
    this->slot = 0;
    this->capacity = fixed_len_page_capacity(page);
    // Initialize the record
    this->current_record = new Record();
 }

 bool PageRecordIterator::hasNext() {
    // the slot number cannot be greater than the capacity
    if (this->slot >= this->capacity)
        return false;

    // Get the next non-empty record in the page
    return read_fixed_len_page(this->page, this->slot, this->current_record);
 }

 Record* PageRecordIterator::next () {
    assert(this->hasNext());

    // Increment the slot number, hasNext() already set the new record
    this->slot++;
    return this->current_record;
 }
 Record* PageRecordIterator::get_current() {
    // This is because I need to access the record in the RecoredIterator class
    return this->current_record;
 }
 /**
  * PageIterator constructor
  * loop through each record of the directory and use attribute 0(page offset)
  * to read the page data from the heapfile
  */
PageIterator::PageIterator(Heapfile* heapfile, Page* directory) {
    this->heapfile = heapfile;
    this->directory = directory;
    // Initialize a record iterator pointer
    PageRecordIterator *itr = new PageRecordIterator(directory);
    this->iterator = itr;
    // This is to discard the header of the directory
    this->iterator->next();
    this->current_page = new Page();
    init_fixed_len_page(this->current_page, this->heapfile->page_size, _calc_directory_page_slot_size());    
}

PageIterator::~PageIterator() {
    _free_page(this->current_page);
}
bool PageIterator::hasNext() {
    return this->iterator->hasNext();
}
Page* PageIterator::next() {
    assert(this->hasNext());
    // Get the page offset of the next record
    this->next_offset = _read_directory_record(this->iterator->get_current(), 0);
    // Read the page data
    _read_page_from_file(this->current_page, next_offset, this->heapfile->file_ptr);
    return this->current_page;
}



/**
 * RecordIterator constructor
 */
RecordIterator::RecordIterator(Heapfile *heapfile) {
    this->heapfile = heapfile;
    DirectoryIterator *itr = new DirectoryIterator(heapfile);
    this->directoryIterator = itr;
    this->pageIterator = NULL;
    this->pageRecordIterator = NULL;
}
RecordIterator::~RecordIterator() {
    if(this->pageIterator)
        delete this->pageIterator;
    if(this->pageRecordIterator)
        delete this->pageRecordIterator;
}
/**
 * Get the next non-empty record in the heap
 */
Record RecordIterator::next() {
    assert(this->hasNext());
    return *this->pageRecordIterator->next();
}

/**
 * Check if the heap has anymore non-empty record
 */
bool RecordIterator::hasNext() {
CHECK_PAGE:
    if(this->pageRecordIterator) {
        if(this->pageRecordIterator->hasNext())
            return true;
        delete this->pageRecordIterator;
        this->pageRecordIterator = NULL;
    }
CHECK_DIRECTORY:
    if(this->pageIterator) {
        if(this->pageIterator->hasNext()) {
            Page *page = this->pageIterator->next();
            this->pageRecordIterator = new PageRecordIterator(page);
            goto CHECK_PAGE;
        }
        delete this->pageIterator;
        this->pageIterator = NULL;
    }
if(this->directoryIterator->hasNext()) {
    Page *directory = this->directoryIterator->next();
    this->pageIterator = new PageIterator(this->heapfile, directory);
    goto CHECK_DIRECTORY;
}
return false;
}
