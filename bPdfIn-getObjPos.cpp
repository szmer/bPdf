size_t bPdfIn::getObjPos(int objNum) {

    for(int i=0; i<(int)xrefSections.size(); i++) {
	if((xrefSections[i].start <= objNum)
	   && (xrefSections[i].end >= objNum)) {
	  
	   // Find the xref section.   
	   file.seekg(xrefSections[i].pos);

	   // Go to the desired entry.
	   file.ignore(xrefSections[i].entryLength * (objNum-xrefSections[i].start));

	   // Get the position (first 10 bytes of the entry, with leading zeros).
	   std::string entry;
	   entry = bPdf::getline(file);
	   entry.erase(10);
	   const size_t byteOffsStart = entry.find_first_not_of('0');
	   if(byteOffsStart == std::string::npos
		|| entry.find_first_not_of("0123456789") != std::string::npos) // something must be REALLY wrong
	   {
		std::cout << "Warning: xref table is corrupted in entry for object: " << objNum << "\n";
		return 0;
	   } 
	   if(byteOffsStart >= 10)			// only zeros in byte offset part
		return 0;

           size_t objPos = (size_t)std::atoi(entry.substr(byteOffsStart).c_str());
	   return objPos;
	} // if objNum is in section's scope
    } // for xrefSections

    // return -1 when nothing is found:
    return -1;
}
