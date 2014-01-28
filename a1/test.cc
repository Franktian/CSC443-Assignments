#include "library.h"
#include <iostream>
#include <cstring>

using namespace std;

// void test_fixed_len_sizeof() {
//     Record *r = new Record();
//     int numattr = 100;
//     char attr[10+1] = "aaaaaaaaaa";
//     while (numattr > 0) {
//         r->push_back(attr);
//         numattr--;
//     }
//     cout << fixed_len_sizeof(r) << endl;
//     delete r;
// }

// void test_fixed_len_page_capacity() {
//     Page *p = new Page();
//     p->page_size = 110;
//     p->slot_size = 10;
//     cout << fixed_len_page_capacity(p) << endl;
//     delete[] p->data;
//     delete p;
// }

void test_memcpy_int2char() {
    int attr1, attr2, attr3;
    attr1 = 0;
    attr2 = 2;
    attr3 = 123456;

    char bytes_buf[sizeof(int)];

    // cout << sizeof(int) << endl;

    for (int i = 0; i < sizeof(int); ++i)
    {
        bytes_buf[i] = (attr3 >> 8*(i)) & 0xFF;
    }

    // memcpy(bytes_buf, &attr1, sizeof(int));

    for (int i = 0; i < sizeof(int); ++i)
    {
        printf("%x\n", bytes_buf[i]);
    }

    int back;
    memcpy(&back, bytes_buf, sizeof(int));

    printf("%d\n", back);

    printf("Test memcpy to and back\n");    
    
    char* bf = new char[sizeof(int)];
    memcpy(bf, &attr3, sizeof(int));

    int back2;
    memcpy(&back2, bf, sizeof(int));

    printf("%d\n", back2);
    delete[] bf;
}

int main() {
    test_memcpy_int2char();
    return 0;
}
