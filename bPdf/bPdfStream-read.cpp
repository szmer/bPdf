std::string bPdfStream::readRaw() {
    char* stream = new char[len];
    source->file.read(stream, len);
    std::string streamStr(stream, len); 
    delete[] stream;
    return streamStr;
}

std::string bPdfStream::read() {
   std::string stream = readRaw();

   int filtCheck = filter;       // copy for reading subsequent filters

   while(filtCheck != 0) {
       switch(filtCheck) {
          case BPDF_FILTER_NONE:
             break;

          case BPDF_FILTER_FLATE:
             stream = bPdfZlib::quick_decomp(stream);
             break;

          default:
             throw "Unsupported stream compression.";
             break;
       }
       filtCheck = filtCheck >> 8; 
   }

   return stream;
}
