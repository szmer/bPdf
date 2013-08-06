void bPdfIn::loadXref(size_t byteOffset&) {
    file.seekg(byteOffset);

    std::string line;

    // discard the line containing "xref" marker
    line = bPdf::getline(file);

    if(line.find("obj") != std::string::npos)
	loadXrefCompressed(byteOffset);
    else
	loadXrefUncompressed();

    return;
}

void bPdfIn::loadXrefCompressed(size_t &byteOffset) {

    dictionary xrefDict = bPdf::unrollDict( extractObject(byteOffset)  );

    // Get structure of entry (W)
    if(xrefDict.count("/W") == 0) {
        std::cout << "Warning: xref stream dict doesn't contain W entry, and is ignored." << std::endl;
        return;
    }
    std::vector<std::string> arr_str = bPdf::unrollArray(xrefDict["/W"]);
    std::vector<int> W;
    for(int i=0, i<arr_str.size(); i++)
        W[i] = std::atoi( arr_str[i].c_str() );

    // Get structure of the table.
    // Odd entries in array are first objects in subsections, even are numbers of objects in that
    // subsection (that is, started by preceding odd entry).
    std::vector<int> sectionsInStream;
    if(xrefDict.count("/Index") == 0) {
        // Default value: [0 SIZE].
        if(xrefDict.count("/Size") == 0) {
            std::cout << "Warning: xref dict doesn't cointain Stream and Info (optional) entry,"
                      << "and is ignored." << std::endl;
            return;
        }
        sectionsInStream.push_back(0);
        sectionsInStream.push_back( std::atoi(xrefDict["/Size"].c_str()) );
    }
    else {
        arr_str = bPdf::unrollArray(xrefDict["/Index"]);
        for(int i=0; i<arr_str.size(); i++)
             sectionsInStream[i]] = std::atoi( arr_str[i].c_str() );
        if(sectionsInStream.size() % 2 != 0) {
             std::cout << "Warning: xref dict has flawed Index entry, and is ignored." << std::endl;
             return;
        }
    }

    if(trailer.empty())
         trailer = xrefDict;

    return;
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

            if(section.end-section.start < 0)
		continue;

	    // Check how many bytes are in entry. Apparently some PDFs violate sacrosanct 20-byte rule.
	    line = bPdf::getline(file);
	    if(line.length() == 20)          // '\n' extracted
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
