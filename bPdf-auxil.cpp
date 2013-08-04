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
	str += 48 + number/p;	// 48 is the ASCII code for "0"
	number -= (number/p) * p;
    }

    return str;
}
