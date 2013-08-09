size_t bPdfIn::getObjPos(int objNum) {

    for(int i=0; i<(int)xrefSections.size(); i++) {
	if((xrefSections[i].start <= objNum)
	   && (xrefSections[i].end >= objNum)) {

           // Is section is compressed?
           if(xrefSections[i].isCompressed) {
               int info, ret;
               ret = xrefSections[i].comprLookup(objNum, info);

               // see bPdf-intern.h
               if(ret > 0 && info != -1) // object is compressed in stream
                    return -2;
               if(info != -1 || ret == -1)  // "not found" on all other entry types
                    return -1;

               return ret;   // object byte offset
           } 
	  
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
		return -1;
	   } 
	   if(byteOffsStart >= 10)			// only zeros in byte offset part
		return -1;

           size_t objPos = (size_t)std::atoi(entry.substr(byteOffsStart).c_str());
	   return objPos;
	} // if objNum is in section's scope
    } // for xrefSections

    // return -1 when nothing is found:
    return -1;
}
