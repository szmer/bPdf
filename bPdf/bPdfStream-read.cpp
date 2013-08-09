std::string bPdfStream::readsomeRaw(int amount) {
    source->file.seekg(streamPosition+streamPointer);

    char* stream = new char[amount];
    source->file.read(stream, amount);
    std::string streamStr(stream, amount); 
    delete[] stream;

    streamPointer += amount;
    if(streamPointer == streamPosition+len)
        streamPointer = 0;

    return streamStr;
}

std::string bPdfStream::readRaw() {
    size_t streamPointerCache = streamPointer;
    streamPointer = 0;
    std::string wholeStream = readsomeRaw(len);
    streamPointer = streamPointerCache;
    return wholeStream;
}

std::string bPdfStream::readsome(int amount) {
   std::string content = readsomeRaw(amount);

   int filtCheck = filter;       // copy for reading subsequent filters

   while(filtCheck != 0) {
       switch(filtCheck) {
          case BPDF_FILTER_NONE:
             break;

          case BPDF_FILTER_FLATE:
             content = bPdfZlib::quick_decomp(content);
             break;

          default:
             throw "Unsupported stream compression.";
             break;
       }
       filtCheck = filtCheck >> 8; 
   }

   return content;
}

std::string bPdfStream::read() {
    size_t streamPointerCache = streamPointer;
    streamPointer = 0;
    std::string wholeStream = readsome(len);
    streamPointer = streamPointerCache;
    return wholeStream;
}
