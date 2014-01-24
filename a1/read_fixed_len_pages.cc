#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/timeb.h>

#include "library.h"

using namespace std;
// Number of bytes taken by a record.
#define RECORD_SIZE 1000

void printrecord(FILE *csv, Record *record) {
    bool first = true;
    for (Record::iterator it = record->begin(); it != record->end(); it++) {
        const char *attr = *it;
        fprintf(csv, (first ? "%s" : ",%s"), attr);
        first = false;
    }   
    fprintf(csv, "\n");
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("%s <page file> <page size>\n", argv[0]);
        exit(1);
    }   

    size_t page_size = atoi(argv[2]);

    FILE *csvf = stdout;
    FILE *pagef = fopen(argv[1], "r");

    if (!csvf || !pagef) {
        printf("Error opening file(s).\n");
        exit(1);
    }   

    cout << "Start the timer" << endl;
    struct timeb _t;
    ftime(&_t);
    long init = _t.time * 1000 + _t.millitm;

    int page_count = 0;
    int record_count = 0;

    Page page;
    init_fixed_len_page(&page, page_size, RECORD_SIZE);

    int capacity = fixed_len_page_capacity(&page);

    fread(page.data, 1, page.page_size, pagef);
    while (!feof(pagef)) {
        page_count++;

        // Print records from the page
        Record r;
        for (int i = 0; i < capacity; ++i) {
        //    if (read_fixed_len_page(&page, i, &r)) {
                read_fixed_len_page(&page, i, &r);
                printrecord(stdout, &r);
                record_count++;
        }

        fread(page.data, 1, page.page_size, pagef);
    }


    ftime(&_t);
    long done = _t.time * 1000 + _t.millitm;
    long time = done-init;

    printf("NUMBER OF RECORDS: %d\n", record_count);
    printf("NUMBER OF PAGES: %d\n", page_count);
    printf("TIME: %ld milliseconds\n", time);

    fclose(pagef);
    return 0;
}


