#include "library.h"
#include <iostream>

using namespace std;

void test_fixed_len_sizeof() {
    Record *r = new Record();
    int numattr = 100;
    char attr[10+1] = "aaaaaaaaaa";
    while (numattr > 0) {
        r->push_back(attr);
        numattr--;
    }
    cout << fixed_len_sizeof(r) << endl;
}

void test_fixed_len_page_capacity() {
    Page *p = new Page();
    p->page_size = 110;
    p->slot_size = 10;
    cout << fixed_len_page_capacity(p) << endl;
}

int main() {
    test_fixed_len_sizeof();
    test_fixed_len_page_capacity();
    return 0;
}
