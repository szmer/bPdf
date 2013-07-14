// returns byte offset of object
// or 0 when failed
unsigned int PdfIn::resolveIndirect(const std::string& reference) {

    if(reference.length() < 5) // nothing to do here
	return 0;

    unsigned int number = 0; // to be extracted
    int w = 0; // number of "words" in reference (delimited with space)
    bool read = false;

    for(int j = 0; j < (reference.length() - 4); j++) {
	for(int i = j; i < reference.length(); ++i) {

	    if(std::isspace(reference[i]) && w < 2) { // space
	    	w++;
	    	continue;
   	    }

	    if(w < 2 && std::isdigit(reference[i])) { // perhaps indirect, keep reading
		if(w == 0) {
	 	    char str[] = {reference[i], '\0'};
		    number = number * 10 + atoi(str);
		}
		continue;
	    }
	    if(w == 2 && reference[i] == 'R') {// ok, this is an indirect
		read = true;
		goto reffound;
	    }

	    // if it is not a reference...
	    break;
	}
	w = 0;
	number = 0;
     }
     reffound:
     if(read == true && number <= xrefData.size())
	return xrefData[number];
     else
	return 0;
}

// overload for above: retrying
unsigned int PdfIn::resolveIndirect(const std::string& reference, int& pos, int& beg) {

   if(reference.length() < 5 || pos >= reference.length()) // nothing to do here
	return 0;

   unsigned int number = 0; // to be extracted
   int w = 0; // number of "words" in reference (delimited with space)
   bool read = false;

   for(int j = pos; j < (reference.length() - 4); j++) {
	beg = j;
	for(int i = j; i < reference.length(); i++) {

  	    if(std::isspace(reference[i]) && w < 2) { // space
		w++;
		continue;
	    }
  	    if(w < 2 && std::isdigit(reference[i])) { // perhaps indirect, keep reading
		if(w == 0) {
	 	    char str[] = {reference[i], '\0'};
			number = number * 10 + atoi(str);
		}
		continue;
	    }
	    if(w == 2 && reference[i] == 'R') {// ok, this is an indirect
		read = true;
		pos = i;
		goto reffound;
	    }
	    // if it is not a reference...
	    break;
	}
	number = 0;
	w = 0;
    }
    reffound:
    if(read == true && number <= xrefData.size())
	return xrefData[number];
    else
	return 0;
}

// extracts raw indirect object from current position
/*std::string PdfIn::extractObject() {

   std::string object = "";
   std::streampos pos;
   bool write = false;

   while(true) {
	pos = file.tellg();
	std::string buff= "";
	std::getline(file, buff);
	buff += '\n';

	if(write == false && buff.find("obj") != std::string::npos) {
	    write = true;
	    file.seekg(-(buff.length()), std::ios::cur);
	    // read again from place after "obj" occurs
	}
	if(write == true) {
	    if(std::size_t endobj = buff.find("endobj") != std::string::npos) {
	    	object += buff.substr(0, endobj-1); // we must also add this fragment
	    	return object;
	    }
	    object += buff; // "else"
	}
    }
}*/
void PdfIn::peekObj(unsigned int pos) {

    std::cout << "Here is object on " << pos << '\n';
    std::string line;

    file.seekg(pos, std::ios::beg);
    for(int i = 0; i < 3; i++) {
	std::getline(file, line);
	std::cout << line << '\n';
    }
    return;
}
PdfIn::PdfIn(const char* filename) {
    file.open(filename, std::fstream::in | std::fstream::ate);
}
PdfIn::PdfInPages::PdfInPages() {
    attributesIndex = -1;
}

