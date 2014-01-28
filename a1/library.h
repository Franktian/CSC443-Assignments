#ifndef __LIBRARY_H_INCLUDED
# define __LIBRARY_H_INCLUDED

#include <vector>
#include <cstdio>

#define SCHEMA_ATTRIBUTE_LEN 10
#define SCHEMA_ATTRIBUTES_NUM 100
#define RECORD_SIZE 1000

typedef const char* V;
typedef std::vector<V> Record;
// The byte offset position in a file
typedef unsigned int Offset;

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record);

/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf, int attrLen);


/**
 * Deserializes from `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record, int attrLen, int numAttr);


/**
* Page
*/
typedef struct {
    void *data;
    int page_size;
    int slot_size;
    int capacity;
} Page;

/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size);

/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page);

/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page);

/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r, int attrLen);

/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r, int attrLen);

/**
 * Read a record from the page from a given slot.
 */
bool read_fixed_len_page(Page *page, int slot, Record *r, int attrLen, int numAttr);

/**
 * Heap file
 */
typedef struct {
    FILE *file_ptr;
    int page_size;
} Heapfile;

#define DIRECTORY_RECORD_ATTRIBUTE_LEN sizeof(Offset)
#define DIRECTORY_RECORD_ATTRIBUTE_NUM 3

/*
 * PageID, unique for a page within a heapfile
 * We use the byte offset of the page as its page id.
 * This is the same as the page_offset data members in
 * RecordID and DirectoryRecord.
 */
typedef Offset PageID;

/** 
 * Data structure defining the ID of a record. 
 * The RecordID must be unique given a heapfile
 */
typedef struct {
    Offset page_offset;
    int slot;
} RecordID;

// A number used to check if the deserialized heapfile is non-empty
#define DIRECTORY_RECORD_SIGNATURE 314159

// The first directory's byte offset, should be 0
#define FIRST_DIRECTORY_OFFSET 0

// The page offset for empty directory record
#define EMPTY_PAGE_OFFSET 0

/* 
 * A drectory header record has 3 attributes:
 * byte offset of this directory
 * byte offset of the next directory
 * A signature number used for validation after parsing
 */
typedef struct {
	Offset offset;
	Offset next;
	Offset signature;
} DirectoryHeader;

/* 
 * A drectory record has 3 attributes:
 * byte offset of the page pointed by this record
 * number of free slots in the page
 * A signature number used for validation after parsing
 */
typedef struct {
	Offset page_offset;
	Offset free_slots;
	Offset signature;
} DirectoryRecord;

/**
 * Initalize a heapfile to use the file and page size given.
 * If the file is a new heapfile, create the first directory page
 */
void init_heapfile(Heapfile *heapfile, int page_size, FILE *file);

/**
 * Allocate another page in the heapfile.  This grows the file by a page.
 */
PageID alloc_page(Heapfile *heapfile);

/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page);

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid);

/**
 * The directory iterator for a heapfile of multiple directories
 */
class DirectoryIterator {
public:
	DirectoryIterator(Heapfile* heapfile);
	~DirectoryIterator();
	bool hasNext();

	// Return the next directory page
	char* next();
private:
	Heapfile* heapfile;
	char* directory;
	DirectoryHeader *header;		// Points to the header record of the current directory
	bool validNext;
};

/**
 * Page Record Iterator
 * Iterate through all the records in a page data structure
 */
class PageRecordIterator {
public:
	PageRecordIterator(Page *page);
	~PageRecordIterator();
	bool hasNext();
	Record* next();
private:
	Record* current_record;
	Record* next_record;
	Page *page;
	int curr_slot;  // Indicate the current slot number
	int next_slot;
	int capacity;  // Indicate the capacity of the page
	bool validNext;
};

/* Page iterator class for a directory 
 * Used to iterate through all pages in a directory
 */
class DirectoryRecordIterator {
public:
	DirectoryRecordIterator(Heapfile* heapfile, char* directory);
	~DirectoryRecordIterator();
	bool hasNext();
	
	// Return the next data page
	DirectoryRecord* next();

private:
	Heapfile* heapfile;
	char* directory;
	DirectoryRecord* directory_record;
	int curr_slot;
	int directory_capacity;
	bool validNext;
};

/* Record iterator class for iterating through 
 * all records in the heap file
 */
class RecordIterator {
public:
    RecordIterator(Heapfile *heapfile);
    ~RecordIterator();

    // Get the next non-empty record in the heap
    Record next();
    
    // Check if the heap has anymore non-empty record
    bool hasNext();

private:
	// The heap file we are iterating
	Heapfile* heapfile;
	
	DirectoryIterator *directory_itr;
	DirectoryRecordIterator *directory_record_itr;
	PageRecordIterator *page_record_itr;
	
	bool validNext;
	DirectoryRecord* curr_dir_record;
	char* curr_dir;
	Page* curr_page;
};

/* Used to iterate through all pages in the heapfile */
class PageIterator {
public:  
	PageIterator(Heapfile *heapfile);
	~PageIterator();
	bool hasNext();
	PageID next();
private:
	Heapfile* heapfile;
	bool validNext;
	char* curr_dir;
	DirectoryIterator *directory_itr;
	DirectoryRecordIterator *directory_record_itr;
};

#endif
