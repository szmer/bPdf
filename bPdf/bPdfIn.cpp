#include "bPdf-structure.h"

bPdfIn::~bPdfIn() {
    file.close();
}

void bPdfIn::doc(const char* filename) {

    // place cursor in the end of file:
    file.open(filename, std::fstream::in | std::fstream::ate);
    if(!file)
        throw "Cannot open PDF file."; 

    // Following little hack leads us directly to the end of "startxref" keyword.
    char chr = '\0';
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

   // Load the cross-reference table. Dictionary of xref stream will be received.
   dictionary lastTrailer = loadXref(xrefPos);

   // Load trailer and previous xref tables and trailers if file was updated. Tables are loaded
   // in reversed chronological order so most recent entry for each object will come first for
   // bPdf::getObjPos(). Old trailers are all discarded.
   while(true) {
       if(lastTrailer["/Type"] != "/XRef") {
          // loadXref() left cursor one line after the last entry of the table. We should go back
          // to extract trailer (when there is no EOL after "trailer" keyword).
           chr = ' ';
           do {
               if(chr == '\n' || chr == '\r')
                    break;
               file.seekg(-2, std::ios::cur);
           } while(file.get(chr));

           lastTrailer = bPdf::unrollDict( extractObject() );
       } // if lastTrailer["/Type"] != "/XRef"

       if(trailer.empty())
            trailer = lastTrailer;

       if(lastTrailer.count("/XRefStm") != 0)          // hybrid-reference file
            loadXref((size_t) atoi(lastTrailer["/XRefStm"].c_str()));

       if(lastTrailer.count("/Prev") == 0)
            break;

       dictionary xrefDict = loadXref( (size_t)atoi(lastTrailer["/Prev"].c_str()) );
       lastTrailer = xrefDict.empty() ? dictionary() : xrefDict;
   }

   if(xrefSections.size() == 0)
	throw "bPdfIn was unable to find any cross-reference tables in file!";
}
