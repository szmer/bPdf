/*

OBJ A ENDOBJ - \s ???

*/


static size_t strfind(const char* needle_cstr, const char* haystack_cstr, const int pos = 0) {

    std::string needle = needle_cstr;
    std::string haystack = haystack_cstr;

    if(needle.length() > haystack.length() || pos >= haystack.length())
	return std::string::npos;

    for(int i=0, j=0, k=-1; i<haystack.length(); i++) {
	if(haystack[i] == needle[j]) {
	    if(j==0)	k=i;
	    j++;
	}
	else if(j!=0)
	    k=-1, j=0;

	if(j == needle.length()) 
	    return k;
    }

    return std::string::npos;
}

std::string PdfIn::extractObject
(bool ignoreStreams, size_t startPos) {


    // CONTAINING OBJECTS: strings, arrays, dictionaries
    // each ends with specifing delimiting character
    const int CO_count = 6;
    const char* containingObjectsOpen[CO_count] = {
		"(\0", "<<\0", "[\0", "<\0", "obj\0", "stream\0"
		};
    const char* containingObjectsClose[CO_count] = {
		")\0", ">>\0", "[\0", ">\0", "endobj\0", "endstream\0"
    		};
    int CO_positions[CO_count] = { -1, -1, -1, -1, -1, -1 };
    // /\ /\ /\ if no object sign will be found on zero position, this array will tell which comes first
    int obj_num = -1; 			// there pointer to ending character will be assigned
    int obj_self_pos = -1; 		// position for "obj", in array replaced by "# # obj" position

    // NON-CONTAINING OBJECTS: booleans, names and numbers
    // each ends with space
    const int nCO_count = 15;
    const char* nonContainingObjects[nCO_count] = {
		"true\0", "false\0", "/\0", "+\0", "-\0",
		"0\0", "1\0", "2\0", "3\0", "4\0", "5\0", "6\0", "7\0", "8\0", "9\0" };
    int nCO_positions[nCO_count] = { -1, -1, -1, -1, -1,
				    -1, -1, -1, -1, -1,
				    -1, -1, -1, -1, -1,};


    // reading variables:
    std::string object;
    std::string buffer;


    // place the pointer
    if(startPos >= 0)
	file.seekg(startPos, std::ios::cur);


    // FIND OBJECTS and RETURN if occured on zero position in string:
    while(file.good()) {
	std::getline(file, buffer);
	bool sthFound = false;
	size_t pos;
	
	// EXAMINE COs
	for(int i=0; i<CO_count; i++) {
	   if( ( pos = strfind(containingObjectsOpen[i], buffer.c_str()) ) != std::string::npos) {

		sthFound = true;

		// for "obj" INCLUDE numbers to the beginning of "# # obj" statement:
		if(i == 4) {			// index for "obj" 
		   obj_self_pos = pos;
		   for(int spaces=0 ; (spaces!=2 && pos!=0) ; pos--)
			if(buffer[pos] == ' ')
			    spaces++;
		}

		CO_positions[i] = pos;

		if(pos == 0) {
		    obj_num = i;
		    break;
		}

	   } // if CO found
	} // for COs

	// BREAK loop if sth found
	if(sthFound)
	    break;

	// EXAMINE nCOs
	for(int i=0; i<nCO_count; i++) {
	   if( ( pos = strfind(nonContainingObjects[i], buffer.c_str()) ) != std::string::npos) {

		if(pos == 0) // return to the whitespace
		    return buffer.substr(0, buffer.find_first_of(" \t\n\v\f\r"));
		
		nCO_positions[i] = pos;
		sthFound = true;

	   } // if nCO found
	} // for nCOs

	// BREAK loop if sth found
	if(sthFound)
	    break;

    } // while file.good() (looking for symbols)
    if(!(file.good()))
	throw "Problem with file when extracting PDF object.";


    // IF nothing found on zero pos :
    // CHECK what came first and RETURN if nCO
    if(obj_num == -1) // if no CO found on zero pos
      for(int i=1; i<buffer.length(); i++) {
	for(int j=0; j<CO_count; j++)
	   if(CO_positions[j] == i) {
		obj_num = j;
		break;
	   }
	if(obj_num != -1) 		// CO found, break and load it
	   break;

	for(int j=0; j<nCO_count; j++)
	    if(nCO_positions[j] == i)
		return
		 buffer.substr( nCO_positions[j], buffer.find_first_of(" \t\n\v\f\r", nCO_positions[j]) );
       } // for possible positions - find the first object


    // IF CO found, LOAD it and RETURN

    if(obj_self_pos != -1) {		// "obj" - indirect object
	// extract the "# # o"(..bj) to avoid re-finding it:
    	object = buffer.substr(CO_positions[obj_num], obj_self_pos);
	buffer = buffer.substr(obj_self_pos+1);
    }
    else {
	// extract the first char to avoid re-finding it:
        object = buffer[CO_positions[obj_num]];
        buffer = buffer.substr(CO_positions[obj_num]+1);
    }
    int level = 0; 			// indicate embedded objects
    while(file.good()) { 		// load line by line

	size_t open, close, stream, pos = 0;
	do {
	    open = strfind(containingObjectsOpen[obj_num], buffer.c_str(), pos);
	    close = strfind(containingObjectsClose[obj_num], buffer.c_str(), pos);
	    stream = ignoreStreams ? std::string::npos : strfind("stream", buffer.c_str(), pos);

	    // IF opening char is before closing & "stream", INCREMENT embedding
	    if( open != std::string::npos
			&& (close == std::string::npos || open < close)
			&& (stream == std::string::npos || open < stream) )
	    {	level++, pos=open+1;
		continue;
	    }

	    // IF ending closing char || "stream" found, RETURN
	    if( (stream < close || close < stream) && level == 0 ) {

		if(stream != std::string::npos && stream < close)
	            return (object + buffer.substr(0, stream));
		
		return (object+buffer.substr(0, close));	// else

	    }

	    // ELSE IF closing char is present, DECREMENT embedding
	    if( close != std::string::npos)
		pos = close, level--;
	} while(open != std::string::npos || close != std::string::npos || stream != std::string::npos);

	object += buffer;
	getline(file, buffer);
    } // while file.good() (loading from file)
    if(!(file.good()))
	throw "Problem with file when extracting PDF object.";
}
