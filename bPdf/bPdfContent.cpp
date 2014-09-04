#include "bPdf-structure.h"

bPdfContent::bPdfContent(bPdfStream src) {
    strm = src;
    strm.rewind();       // read from the beginning
    pnt = 0;
    e = false;
}

array bPdfContent::fetch() {
    array oper;
    size_t oper_end = 0;    // ending character of the operator

    if(data.size() == 0) 
        data = strm.read();

    std::stringstream dataStream;
    std::string * dataloc = new std::string( data.substr(pnt) );
    try 
         { dataStream.str(*dataloc); }
    catch(...) { 
         delete dataloc;
         return array();
    }
    delete dataloc;

    size_t locstart = pnt;

    while(dataStream) {
        while(dataStream.peek() < '!') {            // ignore pointless whitespaces
             dataStream.seekg(1, std::ios::cur);
             if(!dataStream)      // must be end of stream
                 return array();
        }

        if(bPdf::isOperator(data, locstart+dataStream.tellg(), oper_end)) {
           oper.push_back(data.substr(
                    data.find_first_not_of(" \n\r\t\f", locstart+dataStream.tellg()),
                    oper_end-(locstart+dataStream.tellg())+1 ));
           break;
        } // if operator is found
        oper.push_back(bPdf::extractObject(dataStream));
    } // while dataStream state is valid

    if(oper.back() == "")       // clean garbage
         oper.pop_back();

    if(oper_end != 0) 
         pnt = oper_end+1;
    else if(!oper.empty())
         throw "Cannot find content stream Pdf operator properly.";

    return oper;
}
