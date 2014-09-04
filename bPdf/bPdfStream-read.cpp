#include "bPdf-structure.h"

std::string bPdfStream::readsomeRaw(size_t amount) {
   if(amount == -1)
       amount = len*len;

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
    std::string wholeStream = readsomeRaw();
    streamPointer = streamPointerCache;
    return wholeStream;
}

std::string bPdfStream::readsome(size_t amount) {

   if(amount == -1)
       amount = len*len;

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

      // Filters additional data.
      std::vector<dictionary> DecodeParms;
      if(dict.count("/DecodeParms") == 1) {
           if(bPdf::isArr(dict["/DecodeParms"]) == 0) {
               std::vector<std::string> arr = bPdf::unrollArray(dict["/DecodeParms"]);
               for(int i=0; i<arr.size(); i++)
                    DecodeParms.push_back( bPdf::unrollDict(arr[i]) );
           }
           else
               DecodeParms.push_back( bPdf::unrollDict(dict["/DecodeParms"]) );
      } // if DecodeParms are present

      // Apply filters.
      int filtCheck = filter;       // copy for reading subsequent filters
      int f = 0;                    // number of filter in DecodeParms
      while(filtCheck != 0) {
         switch(filtCheck) {
           case BPDF_FILTER_NONE:
             outputContent += content;
             break;

           case BPDF_FILTER_FLATE:
             while(zlib.z_state == Z_OK && zlib.data_pnt < zlib.data->length())
                 outputContent += zlib.decompress( BPDF_ZLIB_CHUNK );
             if(DecodeParms.size() != 0 && ! DecodeParms[f].empty()) {
                 if(DecodeParms[f].count("/Predictor") != 1)
                      break;
                 int predictor = atoi( DecodeParms[f]["/Predictor"].c_str() );
                 if(predictor <= 1)
                      break;

                 int cols = -1, bits = -1, colors = -1;
                 if(DecodeParms[f].count("/Columns") == 1)
                     cols = atoi( DecodeParms[f]["/Columns"].c_str() );
                 if(DecodeParms[f].count("/BitsPerComponent") == 1)
                     bits = atoi( DecodeParms[f]["/BitsPerComponent"].c_str() );
                 if(DecodeParms[f].count("/Colors") == 1)
                     colors = atoi( DecodeParms[f]["/Colors"].c_str() );

                 if(predictor == 2)
                     outputContent = bPdfPred::tiff(outputContent, colors, bits);
                 else { if(predictor > 10)
                     outputContent = bPdfPred::png(outputContent, cols, bits);
                     else
                           throw "Unknown predictor in flate-encoded stream.";
                 } // else (predictor != 2)
             }
             break;

           default:
             throw "Unsupported stream compression.";
             break;
         } // switch filtCheck
         filtCheck = filtCheck >> 8; 
      } // while filtCheck != 0

      if(streamPointer == -1)    // readsomeRaw reset, end of stream
          break;

      f++;
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
    std::string wholeStream = readsome();
    streamPointer = streamPointerCache;
    return wholeStream;
}
