bPdfIn::~bPdfIn() {
    file.close();
}

bPdfIn::bPdfIn(const char* filename) {

    // place cursor in the end of file:
    file.open(filename, std::fstream::in | std::fstream::ate);
    if(!file)
        throw "Cannot open PDF file."; 

    // Following little hack leads us directly to the end of "startxref" keyword.
    char chr;
    do {
        file.seekg(-2, std::ios::cur);
        if(chr == 'f')
             break;
    } while(file.get(chr));

    bPdf::getline(file);          // go to the next line - with xref position

    std::string xrefPosStr;
    xrefPosStr = bPdf::getline(file);
    size_t xrefPos = std::atoi(xrefPosStr.c_str());

    if( xrefPos == 0 )
	throw "bPdfIn was unable to find xref position, probably not a Pdf file.";

   // Load the cross-reference table.
   loadXref(xrefPos);

   // Load trailer and previous xref tables and trailers if file was updated. Tables are loaded
   // in reversed chronological order so most recent entry for each object will come first for
   // bPdf::getObjPos(). Old trailers are all discarded.
   dictionary lastTrailer;
   while(true) {
       // loadXref() left cursor one line after the last entry of the table. We should go back
       // to extract trailer (when there is no EOL after "trailer" keyword).
       chr = ' ';
       do {
            if(chr == '\n' || chr == '\r')
                 break;
            file.seekg(-2, std::ios::cur);
       } while(file.get(chr));

       lastTrailer = bPdf::unrollDict( extractObject() );
       if(trailer.empty())
            trailer = lastTrailer;

       if(lastTrailer.count("/Prev") == 0)
            break;

       loadXref( (size_t)atoi(lastTrailer["/Prev"].c_str()) );
   }

   if(xrefSections.size() == 0)
	throw "bPdfIn was unable to find any cross-reference tables in file!";
}
