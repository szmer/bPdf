std::string bPdfIn::extractObject
(size_t startPos, bool ignoreStreams, bool trim) {
     return bPdf::extractObject(file, startPos, ignoreStreams, trim);
}

std::string bPdf::extractObject(std::istream &source, size_t startPos, bool ignoreStreams, bool trim) {
    // CONTAINING OBJECTS: strings, arrays, dictionaries
    // each ends with specifing delimiting character
    const int CO_count = 6;
    const char* containingObjectsOpen[CO_count] = {
		"(\0", "<<\0", "[\0", "<\0", "obj\0", "stream\0"
		};
    const char* containingObjectsClose[CO_count] = {
		")\0", ">>\0", "]\0", ">\0", "endobj\0", "endstream\0"
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

    // starting position in line will be saved to place the pointer in the stream in the end of the object
    size_t lineStart;

    // reading variables:
    std::string object;
    std::string buffer;


    // place the pointer
    if(startPos != std::string::npos)
	source.seekg(startPos, std::ios::beg);

    // FIND OBJECTS and RETURN if occured on zero position in string:

    while(source.good()) {
	lineStart = source.tellg();
	std::getline(source, buffer);
	bool sthFound = false;
	size_t pos;

	// EXAMINE COs
	for(int i=0; i<CO_count; i++) {
	   if( ( pos = buffer.find(containingObjectsOpen[i]) ) != std::string::npos) {

		sthFound = true;

		// for "obj" INCLUDE numbers to the beginning of "# # obj" statement:
		if(i == 4) {			// index for "obj" 
		   obj_self_pos = pos;
		   for(int spaces=0 ; (spaces!=3 && pos>0) ; pos--)
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

	// BREAK loop if sth found on zero position
	if(obj_num != -1)
	    break;

	// EXAMINE nCOs
	for(int i=0; i<nCO_count; i++) {
	   if( ( pos = buffer.find(nonContainingObjects[i]) ) != std::string::npos) {

		if(pos == 0) {
		// 27.07.13 20:52
		    size_t end; // end of the object
		    switch(i) {
			case 0: end = pos + 4; 		// bool. true
			break;
			case 1: end = pos + 5;		// bool. false
			break;

			case 2:				// names
			    end = pos+1;
			    while(		// see PDF spec, p. 17, but added opening signs
						// because writers careleslly break the standards
				(int)buffer[end] > 32 && (int)buffer[end] < 127 && buffer[end] != '('
				&& buffer[end] != '/' && buffer[end] != '[' && buffer[end] != '<'
			    ) 
				end++;
			    break;

			default:			// numbers
			    end = pos+1;
			    while((int)buffer[end] > 47 && (int)buffer[end] < 58)		// digits
			      end++;

			    // see if it's not an indirect ref:
			    int spcs = 0, digits = 0;
			    for(int z=(int)end; z<(int)buffer.length(); z++) {
				if(buffer[z] == ' ') {
				    spcs++;
				    continue;
				}
				if(spcs == 1 && (int)buffer[z] > 47 && (int)buffer[z] < 58)
				    digits++;
				if(spcs == 2 && buffer[z] == 'R') {
				    end = z+1;
				    break;
				}
				if(spcs > 2)
				    break;
			    }

			    break;
		    } // switch
		    source.seekg(lineStart+end, std::ios::beg); // move cursor to the end of obj
		    return buffer.substr(pos, end-pos);
		}
		
		nCO_positions[i] = pos;
		sthFound = true;

	   } // if nCO found
	} // for nCOs

	// BREAK loop if sth found
	if(sthFound)
	    break;

    } // while source.good() (looking for symbols)
    if(!(source.good())) 
	return std::string("");

    // IF nothing found on zero pos :
    // CHECK what came first and RETURN if nCO
    if(obj_num == -1) // if no CO found on zero pos
      for(int i=1; i<(int)buffer.length(); i++) {
	for(int j=0; j<CO_count; j++)
	   if(CO_positions[j] == i) {
		obj_num = j;
		break;
	   }
	if(obj_num != -1) 		// CO found, break and load it
	   break;

	for(int j=0; j<nCO_count; j++)
	    if(nCO_positions[j] == i) {
		// 27.07.13 20:52
		    size_t end; // end of the object

		    switch(j) {
			case 0: end = i + 4; 		// bool. true
			break;
			case 1: end = i + 5;		// bool. false
			break;

			case 2:				// names
			    end = i+1;
			    while(		// see PDF spec, p. 17, but added opening signs
						// because writers careleslly break the standards
				(int)buffer[end] > 32 && (int)buffer[end] < 127 && buffer[end] != '('
				&& buffer[end] != '/' && buffer[end] != '[' && buffer[end] != '<'
			    ) 
				end++;
			    break;

			default:			// numbers
			    end = i+1;
			    while((int)buffer[end] > 47 && (int)buffer[end] < 58)		// digits
			      end++;

			    // see if it's not an indirect ref:
			    int spcs = 0, digits = 0;
			    for(int z=end; z<(int)buffer.length(); z++) {
				if(buffer[z] == ' ') {
				    spcs++;
				    continue;
				}
				if(spcs == 1 && (int)buffer[z] > 47 && (int)buffer[z] < 58)
				    digits++;
				if(spcs == 2 && buffer[z] == 'R') {
				    end = z+1;
				    break;
				}
				if(spcs > 2)
				    break;
			    }

			    // see if it's not an internal ref:
			    if(i > 4 && buffer.substr(i-4, 4) == "@@@@")
				i -= 4;

			    break;
		    }

		    source.seekg(lineStart+end, std::ios::beg); // move cursor to the end of obj
		    return buffer.substr(i, end-i);
	    }
       } // for possible positions - find the first object

    // IF CO found, LOAD it and RETURN

    if(obj_self_pos != -1) {		// "obj" - indirect object

	if(!trim) {
	    // extract the "# # o"(..bj) to avoid re-finding it:
    	    object = buffer.substr(CO_positions[obj_num], obj_self_pos+1);
	    buffer = buffer.substr(obj_self_pos+1);
	}

	else   // skip whole opening marker
	    buffer = buffer.substr(obj_self_pos+3);  
    }
    else {

	if(!trim) {
	    // extract the first char to avoid re-finding it:
            object = buffer[CO_positions[obj_num]];
            buffer = buffer.substr(CO_positions[obj_num]+1);
	}

	else    // skip whole opening marker
	    buffer = buffer.substr( buffer.substr(CO_positions[obj_num]).find_first_of(" \t\n\v\f\r") );

    }
    int level = 0; 			// indicate embedded objects
    while(source.good()) { 		// load line by line

	size_t open, close, stream, pos = 0;
	do {
	    open = buffer.find(containingObjectsOpen[obj_num], pos);
	    close = buffer.find(containingObjectsClose[obj_num], pos);
	    stream = ignoreStreams ? buffer.find("stream", pos) : std::string::npos;

	    // IF opening char is before closing & "stream", INCREMENT embedding
	    if( open != std::string::npos
			&& (close == std::string::npos || open < close)
			&& (stream == std::string::npos || open < stream) )
	    {	level++, pos=open+1;
		continue;
	    }

	    // IF ending closing char || "stream" found, RETURN
	    if( (stream < close || close < stream) && level == 0 ) {


		if(stream != std::string::npos && stream < close) {
		    source.seekg(-(buffer.length()-stream+1), std::ios::cur); // place the pointer on start of stream
	            return (object + buffer.substr(0, stream).erase(stream)); 	// WITHOUT "s"(tream)
		}

		source.seekg(-(buffer.length()-close), std::ios::cur); // place the pointer after the end of obj

		if(trim)		
		return object + buffer.substr(0, close).erase(close);
		// else return WITH whole ending delimiter
		return (object   +   
				buffer.substr(0, close).erase(close).append(containingObjectsClose[obj_num]));
	    }

	    // ELSE IF closing char is present, DECREMENT embedding
	    if( close != std::string::npos)
		pos = close+1, level--;
	} while(open != std::string::npos || close != std::string::npos || stream != std::string::npos);

	object += buffer + "\n";
	getline(source, buffer);
    } // while source.good() (loading from source)
    if(!(source.good()))
	return std::string("");
}

std::string bPdfIn::extractStream(size_t length, size_t pos) {

    if(pos != 0)
        file.seekg(pos, std::ios::beg);

    std::string stream;
    stream.reserve(length);
    for(size_t i = 0; i<length; i++)
	stream += file.get();

    return stream;
}