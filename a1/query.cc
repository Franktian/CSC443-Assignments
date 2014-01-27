#include "library.h"
#include <sys/timeb.h>

#define RECORD_SIZE 1000

bool checkedAlready(char* str, FILE *f){
    long position = ftell(f);
    int substring_length = 5;

    char* _subsring = (char*)malloc(sizeof(char) * substring_length);
    int byteRead = fread(_subsring, sizeof(char), substring_length, f);

    while(byteRead > 0){
        if (strcmp(_subsring, str) == 0){
            return true;
        }

        free(_subsring);
        _subsring = (char*)malloc(sizeof(char) * substring_length);
        byteRead = fread(_subsring, sizeof(char), substring_length, f);
    }
    return false;
}


int main( int argc, const char* argv[] )
{
  
    // Process the input parameters.
    if (argc != 5){
	cout << "ERROR: invalid input parameters!" << endl;
        cout << "Please put: <heapfile> <start> <end> <page_size>" << endl;
        return 0;
     }

    const char* heapfile_name = argv[1];
    char* start = (char*)argv[2];
    char* end = (char*)argv[3];
    int page_size = atoi((argv[4]));

    // SQL: 
    // SELECT SUBSTRING(A2, 1, 5), COUNT(*) FROM T
    // WHERE A1 >= start AND A1 <= end
    // GROUP BY SUBSTRING(A2, 1, 5) 

    // Open the heapfile and extract data
    FILE *hFile = fopen(heapfile_name, "r");
    if (!hFile) {
	cout << "ERROR: failed to open the heapfile!" << endl;
	exit(1);
    }    

    Heapfile *heapfile = new Heapfile();
    heapfile->page_size = page_size;
    heapfile->file_ptr = hFile;

    // Prepare records
    Record cur;
    for(int i = 0; i < SCHEMA_ATTRIBUTES_NUM; i++){
        char content[10] = "         ";
        cur.push_back(content);
    }

    // *** RecordIterator* iterator = (RecordIterator*)malloc(sizeof(RecordIterator));
    RecordIterator* iterator = new RecordIterator(heapfile, RECORD_SIZE, page_size);
    // *** init_record_iterator(iterator, heapfile, RECORD_SIZE, page_size);

    FILE *T = fopen("view", "w+");
    if (!T) {
	cout << "ERROR: Failed to open the view file!" << endl;
	exit(1);
    }	

    int numRec = 0;
    // *** while(iterator->hasNext){
    while(iterator->hasNext()){
        //*** iterate_record(iterator);  
        cur = iterator->next();  
    
        // *** read_current_record(iterator, &cur);
        // A1 = cur.at(0)
        // WHERE A1 >= start AND A1 <= end
        if (cur.at(0)[0] >= start[0] && cur.at(0)[9] <= end[0]){
            // A2 = cur.at(1)
            // Save queried to the view file
            fwrite(&cur.at(1)[1], sizeof(char), 1, T);
            fwrite(&cur.at(1)[2], sizeof(char), 1, T);
            fwrite(&cur.at(1)[3], sizeof(char), 1, T);
            fwrite(&cur.at(1)[4], sizeof(char), 1, T);
            fwrite(&cur.at(1)[5], sizeof(char), 1, T);
        }
    
        numRec++;
    }
    // Heap file no longer needed
    fclose(hFile);

    // Process the "Group By" Clause.
    int substring_length = 5;

    long T_length = ftell(T);

    rewind(T);
  
    FILE *temp = fopen("_view", "w+");

    int total_read = 0;
    while (total_read < T_length){
        char* substring = (char*)malloc(sizeof(char) * substring_length);
        fread(substring, sizeof(char), substring_length, T);
        long cursor_position = ftell(T);
        // Let us count the number of occurance!
        rewind(T);
        int total_read_1 = 0;
        int count = 0;
        while (total_read_1 < T_length){
            char* _subsring = (char*)malloc(sizeof(char) * substring_length);
            fread(_subsring, sizeof(char), substring_length, T);
            int comp = strcmp(substring, _subsring);
            if (comp == 0 && !checkedAlready(substring, temp)){
                count++;
            }
            total_read_1 += substring_length;
            free(_subsring);
        }
        // Write to a temp file
        fwrite(&substring, sizeof(char), substring_length, temp);
        // fwrite(&count, sizeof(int), 1, temp);
        printf("%s ", substring);
        printf("%d\n", count);

        fseek(T, cursor_position, SEEK_SET);
        total_read+=substring_length;
        free(substring);
    }

    fclose(temp);
    fclose(T);
    printf("NUMBER OF RECORDS : %d\n", numRec);
    return 0;
}
