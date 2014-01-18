#include "library.h"
#include <iostream>

int main() {
    Record *r = new Record();
    int numattr = 100;
    char* attr = "aaaaaaaaaa";
    while (numattr > 0) {
        r->push_back(attr);
        numattr--;
    }
    cout << fixed_len_sizeof(r);
    return 0;
}
