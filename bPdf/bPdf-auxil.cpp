#include "bPdf-structure.h"

std::string bPdf::itoa(int number) {
    std::string str;

    if(number == 0)
	return std::string("0");
    if(number < 0) {	// if negative, make it positive
	str += '-';
	number = 0-number;
    }

    // find the power of 10:
    int p = 1;
    while(number / p > 0)
	p *= 10;
    p /= 10;

    for(; p!=0; p/=10) {
	str += '0' + number/p;
	number -= (number/p) * p;
    }

    return str;
}

// Version of getline aware of PDF newline conventions: either \n, \r\n or \r
// are permitted. Returned line is not trimmed - does have newline characters
// in its end.
std::string bPdf::getline(std::istream &source) {
    std::string line;
    char chrct;

    while( source.get(chrct) ) {
        line += chrct;

        if(chrct == '\n')
             break;

        if(chrct == '\r') {
             if( ! source.eof() && source.peek() == '\n')
                   line += source.get();
             break;
        }
    }
    return line;
}
