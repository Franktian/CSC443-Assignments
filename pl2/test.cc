#include <cstdio>
#include <iostream>
#include <cstring>

using namespace std;

long _get_eof_offset(FILE* file) {
    fseek(file, 0, SEEK_END);
    return ftell(file);
}

int main( int argc, const char* argv[] ) {
	char str[] = "EricZhu";
	FILE *input = fopen("testinput.txt", "r");
	FILE *output = fopen("testoutput.txt", "w");

	long filesize = _get_eof_offset(input);

	cout << "File size = " << filesize << endl;

	char* buf = new char[9];

	fseek(input, 0, SEEK_SET);
    fread(buf, 9, 1, input);

    for (int i = 0; i < 9; i++)
    {
    	if (buf[i] == '\n') {
    		cout << "new line" << endl;
    	}
    	else {
    		cout << buf[i] << endl;
    	}
    }

    fseek(output, 0, SEEK_SET);
    fwrite(buf, 1, 9, output);

    delete[] buf;
    fclose(input);
    fclose(output);
}