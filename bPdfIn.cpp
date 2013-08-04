bPdfIn::~bPdfIn() {
    file.close();
}

bPdfIn::bPdfIn(const char* filename) {

    file.open(filename, std::fstream::in | std::fstream::ate);
    // we've placed cursor in the end of file

    /* Find xref table position information */
    int linesFromEnd = 0;

    for(int i=0; i<100; i++) {
	file.seekg(-2, std::ios::cur);
	if(file.get() == '\n')
	     linesFromEnd++;
	if(linesFromEnd == 2)
	      break;
    } // 100 times

    if( linesFromEnd != 2 || file.fail() )
	throw "bPdfIn was unable to find xref position, probably not a Pdf file.";

    std::string xrefPosStr;
    std::getline(file, xrefPosStr);
    size_t xrefPos = std::atoi(xrefPosStr.c_str());

   /* Get the file trailer dictionary. */
   // Find the leading '<<' dictionary marker, then extract it.
   for(int i=0;; i++) {
	file.seekg(-2, std::ios::cur);
	if(file.get() == '<') {
	     if(file.get() == '<')
		break;
	     else
		file.seekg(-1, std::ios::cur);
	} // if character == '<'

	if( i == 1200 || file.fail() )
	     throw "bPdfIn was unable to find file trailer, probably not a Pdf file.";
   } // 1200 times
  
   file.seekg(-2, std::ios::cur); 
   trailer = bPdf::unrollDict( extractObject(file.tellg()) );

    /* Read xref table.
     * Note that we assume that we have one xref table which is not packed in stream.
     * It is most common situation, but handling other cases remains to be implemented
     * later.
     */
    file.seekg(xrefPos);

    // ignore "xref" marker
    while(file.get() != '\n');

    std::string line;

    while(true) {
	std::getline(file, line);

	if(line == "")
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
	    section.end = std::atoi(line.substr(spcPos+1).c_str());
	    section.pos = file.tellg();
	    xrefSections.push_back(section);

	    file.ignore((section.end-section.start)*20); // ignore computed number of
							 // 20-bytes entries (for now)
        } // if line introduces subsection properly

	else
	     break;
    } // while true

    if(xrefSections.size() == 0)
	throw "bPdfIn was unable to find any cross-reference tables in file!";
}
