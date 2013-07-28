// public, calls all following functions
void PdfIn::mapDocument() {

	checkCR();
	file.seekg(-1, std::ios::cur);

	unsigned int xrefPos = getXrefPos();

	getTrailer();

	getXrefData(xrefPos);
}

void PdfIn::checkCR() {

	char readChar = 'x';

	for(int i = 0; i < 11; ++i) { // !!! loop is in fact terminated from its body (see below)

		if(i > 9) {
			PDFIN_FATAL("Error while checking new line style in PDF. Program terminated. \n");
			exit(1);
		}

		file.seekg(-1, std::ios::cur);
		file.get(readChar);
		file.unget();

		if(readChar == '\n') {
			file.seekg(-2, std::ios::cur);
			file.get(readChar);

			if(readChar == '\r')
				CR = true;
			else
				CR = false;

			break;
		}
	}
}

unsigned int PdfIn::getXrefPos() { 
	// placing pointer where byte offset info is
	if(CR == false)
		file.seekg(-7, std::ios::end);
	else
		file.seekg(-9, std::ios::end);

	//
	////!!! kwalifikuje się do optymalizacji (?) z użyciem bufora
	//
	char readChar[] = { 'x', '\n' }; // quasi-string for std::atoi()
	unsigned int xrefPos = 0;

	for(int i = 0; file.get(readChar[0]); ++i) { // getting digit by digit...

		if(std::isdigit(readChar[0]))
			xrefPos = xrefPos + (std::atoi(readChar) * pow(10,i)); // ...and building a number
		else { if(i != 0)
			break;
		else
			i--;
		}

		file.seekg(-2, std::ios::cur); // moving pointer to the previous char
	}
	
	return xrefPos;
}

void PdfIn::getTrailer() {

	// seek for opening "<<" of trailer dictionary:
	file.seekg(-10, std::ios::cur);
	std::streampos pos = file.tellg();

	char readChar;
	int level = -1; // level of nesting (see also descr. in pdfDict.cpp)
	while(true) {

		file.get(readChar);

		if(readChar == '<') {

			file.seekg(-2, std::ios::cur);
			file.get(readChar);
			if(readChar == '<') {
				if(level == 0) { // that is!
					break;
				}
				else
					level--;
			}
		}
		else if(readChar == '>') { // detects start of a dictionary, nested one should be ignored

			file.seekg(-2, std::ios::cur); 
			file.get(readChar);
			if(readChar == '>')
				level++;
		}
		file.seekg(-2, std::ios::cur);
	}

	// read trailer dictionary:
	file.seekg(-2, std::ios::cur); // we must move back pointer to get both opening brackets "<<"
	std::string trailerDict = ""; // the source for pdfDictEntry(): 

	while(file.tellg() != pos) {
		file.get(readChar);
		trailerDict += readChar;
	}

	// build vector containing trailer data:
	trailer = unrollDict(trailerDict);

	return;
}

void PdfIn::getXrefData (unsigned int xrefPos) {
	
	file.seekg(xrefPos + 4, std::ios::beg); // moving pointer straight to the xref table

	int start = 0; // first number of first object in xref subsection
	int number = 0; // overall number of objects in the subsection
	bool read = false; // used to indicate if start or number has already changed
	char readChar[] = {'x', '\0'};

	// get "start":
	for(int i = 0; i < 5; i++) {
		file.get(readChar[0]);

		if(isdigit(readChar[0])) {
			start = (start * 10) + std::atoi(readChar);
			read = true;
		}

		else if(read == true)
				break;
	}

	read = false;

	// get "number":
	for(int i = 0; i < 5; i++) {
		file.get(readChar[0]);

		if(std::isdigit(readChar[0])) {
			number = (number * 10) + std::atoi(readChar);
			read = true;
		}

		else if(read == true)
				break;
	}

	file.seekg(-1, std::ios::cur);

	for(int i = 0; i < 5; i++) { // just move to the first entry

		file.get(readChar[0]);

		if(readChar[0] == '\n')
			break;
		if(i == 4)
			return;
	}

	if(trailer.count("/Size") > 0) //reserve memory space for all objects once
		xrefData.reserve( atoi(trailer["/Size"].c_str()) );

	// load into array:
	for(int i = start; i < number; i++) {

		char load[20];
		file.getline(load, 20); // get entry
		read = false;

		char entry[11] = "mrocznysek"; // must replace memory garbage for std::atoi()
		for(int j = 0, k = 0; j < 20; j++) { // format entry
			
			if
			(j < 10 && ((load[j] != '0') || (read == true))) {
				entry[k] = load[j];
				k++;
				read = true;
			}
		}
		xrefData.push_back(static_cast<unsigned int>(std::atoi(entry)));
		for(int j = 0; j < 11; j++)
			entry[j] = 'a';
	}

	// return true;
}
