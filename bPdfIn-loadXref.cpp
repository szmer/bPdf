void bPdfIn::loadXref(size_t byteOffset) {
    file.seekg(byteOffset);

    // discard the line containing "xref" marker
    bPdf::getline(file);

    std::string line;

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
	    section.start = std::atoi(line.substr(0,spcPos).c_str());
	    section.end = section.start + std::atoi(line.substr(spcPos+1).c_str()) - 1;
	    section.pos = file.tellg();

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
