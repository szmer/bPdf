std::string bPdfStream::readsomeRaw(size_t amount) {
    if(amount > len-streamPointer)
        amount = len-streamPointer;

    source->file.seekg(streamPosition+streamPointer);

    char* stream = new char[amount];
    source->file.read(stream, amount);
    std::string streamStr(stream, amount); 
    delete[] stream;

    streamPointer += amount;
    if(streamPointer >= len)
        streamPointer = -1;

    return streamStr;
}

std::string bPdfStream::readRaw() {
    size_t streamPointerCache = streamPointer;
    streamPointer = 0;
    std::string wholeStream = readsomeRaw(len);
    streamPointer = streamPointerCache;
    return wholeStream;
}

std::string bPdfStream::readsome(size_t amount) {

   // Output.
   std::string outputContent;
   if(unconsumed.length() > 0) {
        outputContent += unconsumed.substr(0, amount);
        if(amount < unconsumed.length())
              unconsumed = unconsumed.substr(amount);
        else
              unconsumed.clear();
   }

   while(outputContent.length() < amount && streamPointer != -1) {
      // Input.
      std::string content = readsomeRaw(amount-outputContent.length());

      // Necessary initializations.
      if(zlib.data == NULL)            // zlib has been yet unitialized (guaranteed its by construct)
           zlib = bPdfZlib(&content);
      else {                            // just give fresh data
           zlib.data = &content;
           zlib.data_pnt = 0;
      }

      // Apply filters.
      int filtCheck = filter;       // copy for reading subsequent filters
      while(filtCheck != 0) {
         switch(filtCheck) {
           case BPDF_FILTER_NONE:
             outputContent += content;
             break;

           case BPDF_FILTER_FLATE:
             while(zlib.z_state == Z_OK && zlib.data_pnt < zlib.data->length())
                 outputContent += zlib.decompress(262144);  // 256 kb
             break;

           default:
             throw "Unsupported stream compression.";
             break;
         } // switch filtCheck
         filtCheck = filtCheck >> 8; 
      } // while filtCheck != 0

      if(streamPointer == -1)    // readsomeRaw reset, end of stream
          break;
   } // while outputContent.length() < amount

   if(outputContent.length() > amount) {
       unconsumed = outputContent.substr(amount);
       outputContent = outputContent.substr(0, amount);
   }
   return outputContent;
}

std::string bPdfStream::read() {
    size_t streamPointerCache = streamPointer;
    streamPointer = 0;
    std::string wholeStream = readsome(len*len);
    streamPointer = streamPointerCache;
    return wholeStream;
}
