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

int main() {
    test_fixed_len_sizeof();
    return 0;
}
