dictionary bPdfIn::loadXref(size_t byteOffset) {
    file.seekg(byteOffset);

    std::string line;
    dictionary dict;

    line = bPdf::getline(file);

    if(line.find("obj") != std::string::npos)
	dict = loadXrefCompressed(byteOffset);
    else
	loadXrefUncompressed();

    return dict;
}

dictionary bPdfIn::loadXrefCompressed(size_t byteOffset) {

    bPdfStream xref(byteOffset, this);
    dictionary emptyDict;

    // Get structure of entry (W)
    if(xref.get("/W") == "") {
        std::cout << "Warning: xref stream dict doesn't contain W entry, and is ignored." << std::endl;
        return emptyDict;
    }
    std::vector<std::string> arr_str = bPdf::unrollArray(xref.get("/W"));
    std::vector<int> W;
    int entryLength = 0;        // we'll use it while constructing section structs
    for(int i=0; i<arr_str.size(); i++) {
        W.push_back( std::atoi(arr_str[i].c_str()) );
        entryLength += W[i];
    }

    // Get structure of the table.
    // Odd entries in array are first objects in subsections, even are numbers of objects in that
    // subsection (that is, started by preceding odd entry).
    std::vector<int> sectsInStrm;
    if(xref.get("/Index") == "") {
        // Default value: [0 SIZE].
        if(xref.get("/Size") == "") {
            std::cout << "Warning: xref dict doesn't cointain Stream and Info (optional) entry,"
                      << "and is ignored." << std::endl;
            return emptyDict;
        }
        sectsInStrm.push_back(0);
        sectsInStrm.push_back( std::atoi(xref.get("/Size").c_str()) );
    } // if xref dict has Info entry
    else {
        arr_str = bPdf::unrollArray(xref.get("/Index"));
        for(int i=0; i<arr_str.size(); i++)
             sectsInStrm.push_back( std::atoi(arr_str[i].c_str()) );
        if(sectsInStrm.size() % 2 != 0) {
             std::cout << "Warning: xref dict has flawed Index entry, and is ignored." << std::endl;
             return emptyDict;
        }
    } // else (xref dict has no Index entry)

    // Finally, load xref info and save.
    std::string xrefInfo = xref.read();

    size_t loadedDataPtr = 0; 
    for(int i=0; i<sectsInStrm.size(); i+=2) {
       bPdfXrefSection section;

       section.isCompressed = true;
       section.start = sectsInStrm[i];
       section.end = section.start+sectsInStrm[i+1]-1;

       section.entryLength = entryLength;
       section.entryStructure = W;
       section.data = xrefInfo.substr(loadedDataPtr, sectsInStrm[i+1]*entryLength);
       loadedDataPtr += sectsInStrm[i+1]*entryLength + 1;

       xrefSections.push_back(section); 
    }

    return xref.dict;
}

void bPdfIn::loadXrefUncompressed() {
    std::string line;

    // Normal, uncompressed xref table.
    while(true) {
	line = bPdf::getline(file);

	// trim whitespaces if present
	if(line.length() > 0)
	    line = line.substr(0, line.find_last_not_of("\r\t\n ")+1);

	if(line.length() == 0)
	    break;

	// Validate string: should contain exactly two numbers and one space in between.
	size_t spcPos = line.find_first_of(' ');
	if(
	    ( spcPos
		== line.find_last_of(' ')
		!= std::string::npos )
	    && (line.substr(0,spcPos).find_first_not_of("0123456789") == std::string::npos)
	    && (line.substr(spcPos+1).find_first_not_of("0123456789") == std::string::npos)
	  )
	{
	    bPdfXrefSection section;
            section.isCompressed = false;
	    section.start = std::atoi(line.substr(0,spcPos).c_str());
	    section.end = section.start + std::atoi(line.substr(spcPos+1).c_str()) - 1;
	    section.pos = file.tellg();

            if(section.end-section.start < 0)   // empty table, used in hybrid reference files
		break;

	    // Check how many bytes are in entry. Apparently some PDFs violate sacrosanct 20-byte rule.
	    line = bPdf::getline(file);
	    if(line.length() == 20)
		section.entryLength = 20;
	    else {
		if(line.length() == 19) {
		    section.entryLength = 19;
		    std::cout << "Warning: xref entries are only 19 bytes long.\n";
		}
		else {
		     std::cout << "Warning: xref entries seem to have a weird length.\n";
		     break;
		}
	    } // if entry length isn't sane

	    xrefSections.push_back(section);

	    file.ignore((section.end-section.start)*section.entryLength); // ignore computed number of
							                    // entries (for now)
        } // if line introduces subsection properly

	else
	     break;
    } // while true - load xref table
    return;
}
