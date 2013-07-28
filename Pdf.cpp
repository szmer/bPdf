// unrollDict
// if specific "neededKey" is provided, method will read dictionary only unless this key is found
std::map<std::string, std::string> Pdf::unrollDict(std::string str) {

    dictionary dict;
    std::stringstream strStream(str);

    // get rid of opening "<<":
    while(strStream.get() != '<');
    strStream.ignore();

    while(strStream.good()) {
	std::string key = Pdf::extractObject(strStream);	
	std::string value = Pdf::extractObject(strStream);
	if(key != "" && value != "")
	     dict.insert( std::pair<std::string, std::string>(key, value) );
    }

    return dict;
}

// unrollArray()
std::vector<std::string> Pdf::unrollArray(std::string &str) {

	int i = 0;
	std::vector<std::string> array;
	if(str == "")
	    return array;

	while(i != -1) {
	
		std::string element = "";

		i = entry(str, i, element, "[", "]");

		if(i == -2) {
//			PDFIN_NOTICE("Cannot process a array. \n");
			exit(1); // return 0;
		}

		if(element != "")
			array.push_back(element);
		else
			break;
	}

	return array;
}

// function entry()
// assigns next object in source to entry
// no further processing of entry is made (will be received as is in source)
// source must contain whole container objects, including delimiting signs
// processes one entry per call from source - shall be called until -1 (end) is returned
// first time i parameter shall be 0, then shall be equal returned int
// returns final position of pointer, or -1 when end of object is found, or -2 on error.
int Pdf::entry(const std::string &source, int &i, std::string &entry, std::string ssign, std::string esign) {

	entry = "";

	// for reading:
	char readChar[] = { ' ', '\0' };

	/* seek start of object */
	if(i == 0) { 
	
		if(ssign.length() > 2 || esign.length() > 2) {
	#ifdef PDFIN_DEBUG
			std::cout << "Container object: too long starting and/or ending sign provided." << std::endl;
	#endif
			return -2;
		}

		for(; i < (source.length() - 4); ++i) {
			readChar[0] = source.at(i);

			if(readChar[0] == ssign[0]) {
				i++; 
				if(ssign.length() == 1) // 1-char
					break;
				// 2-char:
				readChar[0] = source.at(i++);
				if(readChar[0] == ssign[1]) 
					break;
			}

			else if(i == (source.length() - 5)) {// apparently there is no container in source
	#ifdef PDFIN_DEBUG
			std::cout << "Container object: cannot find specified object in given string." << std::endl;
	#endif
				return -2;
			}
		}
	}
	char snested[] = { '(', '[', '<' };
	char enested[] = { ')', ']', '>' };
	int level = 0;
	// /\ is incremented when function encounters opening sign of PDF objects like: '(' - for literal string, '<' for hexadecimal string,
	// '[' for array, '<<' for (nested) dictionary. These objects allow white-space chars in their bodies and sometimes 
	// can even be nested many times. When any ending sign is encountered, this variable is decremented (when = 0 it doesn't tolerate white-space
	// chars). Note that it can always be ANY closing sign, so some level of responsibility of the PDF writer is assumed
	// (like in many other places...).

	bool naked = false; // for object without limiting character (integer, name) - any limiting character
				// shall end processing

	// get value of entry:
	for(; i < source.length(); ++i) {

		readChar[0] = source.at(i);

		// detect nested objects:
		for (int j = 0; j < 3; j++) {

			if(readChar[0] == snested[j]) {
				if(naked == true)
					return i;
				level++;
				if(readChar[0] == '<' && source.at(i+1) == '<') {// dict sign is "<<"
					entry += '<';
					i++;
				}
				break;
			}

			if(level > 0 && readChar[0] == enested[j]) {
				if(readChar[0] == '>') {
					if(source.at(i+1) == '>') // dict sign is ">>"
						break;
					else if(level == 1 && esign[0] == '>') {
						// otherwise end of whole container would be "recognized"
						entry += '>';
						return ++i;
					}
				}
				level--;
				break;
			}
		}
		if(naked == false && level == 0 && entry != "") // indicate "naked" object
			naked = true;
		if(naked == true && readChar[0] == '/')
			return i;

		// backslash - escape character:
		if(readChar[0] == '\\') {
		   	entry += readChar[0];
			readChar[0] = source.at(++i);
			entry += readChar[0];
			continue;	
		}

		// indirect object references:
		if(std::isdigit(readChar[0])) {
			std::string littleBuff = ""; // little buffer
			int w = 0; // number of "words" in reference (delimited with space)
			int ki = i; // keep i entry (see few lines below
			for(; i < source.length(); ++i) {

				readChar[0] = source.at(i);
				littleBuff += readChar[0];

				if(std::isspace(readChar[0]) && w < 2) { // space
					w++;
					continue;
				}

				if(w < 2 && std::isdigit(readChar[0])) { // perhaps indirect, keep reading
					continue;
				}
				if(w == 2 && readChar[0] == 'R') { // ok, this is an indirect
					entry += littleBuff;
					break;
				}

				// else...
				i = ki;
				readChar[0] = source.at(i); // return to the previous entry
				entry += readChar[0];
				break;
			}
			continue;
		}

		if(level == 0 && readChar[0] == esign[0]) {// end of object
			return -1;
		}

		if(level == 0 && std::isspace(readChar[0])) { // end of entry
			if(entry == "")
				continue;
			i++;
			break;
		}

		entry += readChar[0];
	}
	if(entry == "") {
	#ifdef PDFIN_DEBUG
			std::cout << "Container entry: empty entry obtained." << std::endl;
	#endif
		return -2;
	}

	return i;
}

std::string Pdf::rollDict(std::map<std::string, std::string> &dictMap) {

	std::string dict = "<< ";
	for(std::map<std::string, std::string>::iterator i = dictMap.begin(); i != dictMap.end(); i++) 
		dict += ((*i).first + ' ' + (*i).second + '\n') + ' ';

	return (dict+" >>");
}

std::string Pdf::rollArray(std::vector<std::string> &arrVect) {

	std::string array = "[ ";
	for(int i = 0; i < arrVect.size(); i++)
		array += (arrVect[i] + ' ');

	return (array + " ]");
}
