bPdfStream::bPdfStream(size_t & byteOffset, bPdfIn * src) {
    source = src;

    dict = bPdf::unrollDict( src->extractObject(byteOffset, true, true) );

    // Stream length.
    // This should also throw exception if someone accidentally passed here non-stream object.
    if(dict.count("/Length") == 1)
         len = std::atoi( dict["/Length"].c_str() );
    else
         throw "Stream dictionary without Length entry!\n";

    // Filter (encoding of stream content).
    if(dict.count("/Filter") == 0)
        filter = BPDF_FILTER_NONE;
    else {
        filter = 0;      // for bit shifts
        std::vector<std::string> flts;

        if(dict["/Filter"].find_first_of("[") == std::string::npos) // only one filter
           flts.push_back(dict["/Filter"]);                      // (as name)

        else
           flts = bPdf::unrollArray( dict["/Filter"] );

        for(int i=0; i<flts.size(); i++)
           do {    // "switch"
             if(flts[i]=="/ASCIIHexDecode") {
               filter = (filter<<8)|(int)BPDF_FILTER_ASCIIHEX;
               break; }
             if(flts[i]=="/ASCII85Decode") {
               filter = (filter<<8)|(int)BPDF_FILTER_ASCII85;
               break; }
             if(flts[i]=="/LZWDecode") {
               filter = (filter<<8)|(int)BPDF_FILTER_LZW;
               break; }
             if(flts[i]=="/FlateDecode") {
               filter = (filter<<8)|(int)BPDF_FILTER_FLATE;
               break; }
             if(flts[i]=="/RunLengthDecode") {
               filter = (filter<<8)|(int)BPDF_FILTER_RUNLENGTH;
               break; }
             if(flts[i]=="/CCITTFaxDecode") {
               filter = (filter<<8)|(int)BPDF_FILTER_CCITTFAX;
               break; }
             if(flts[i]=="/JBIG2Decode") {
               filter = (filter<<8)|(int)BPDF_FILTER_JBIG2;
               break; }
             if(flts[i]=="/DCTDecode") {
               filter = (filter<<8)|(int)BPDF_FILTER_DCT;
               break; }
             if(flts[i]=="/JPXDecode") {
               filter = (filter<<8)|(int)BPDF_FILTER_JPX;
               break; }
             if(flts[i]=="/Crypt") {
               filter = (filter<<8)|(int)BPDF_FILTER_CRYPT;
               break; }
             //default:
               filter = (filter<<8)|(int)BPDF_FILTER_UNKNOWN;
          } while(false);// switch flts[i] (for i<flts.size())
    } // else (Filter entry is present)

    // Position of stream in source.
    // extractObject() left cursor in the beginning of "stream" keyword.
    while(src->file.get() != 'm');           // strea'm' <==
    // Keyword should be followed by EOL.
    src->file.ignore();                      // /n or /r
    streamPosition = src->file.tellg();
    if(src->file.peek()=='\n')                      // /r/n?
        streamPosition++;

    streamPointer = 0;
}
