std::string bPdfIn::getObjByNum(int objNum, bool trim, bool ignoreStreams) {

    // If object is uncompressed, it is rather easy.
    int objPos = getObjPos(objNum);
    if(objPos > -1)
        return extractObject(objPos, trim, ignoreStreams);

    // On no such object at all.
    if(objPos == -1) {
        std::cout << "Warning: request for object which is not resolvable." << std::endl;
        return "";
    }

    // If object is compressed... well.
    size_t streamByteOffs;
    int numInStream;
    for(int i=0; i<(int)xrefSections.size(); i++) {

	if((xrefSections[i].start <= objNum) && (xrefSections[i].end >= objNum)
            && xrefSections[i].isCompressed) {

               // Find object stream - look up number given comprLookup in xref table.
               streamByteOffs = getObjPos( xrefSections[i].comprLookup(objNum, numInStream) );
               bPdfStream objStream(streamByteOffs, this);

               const int firstObjByteOffs = std::atoi( objStream.get("/First").c_str() );
               std::string objPositions = objStream.readsome(firstObjByteOffs);

               // Find the proper byte offset.
               int objByteOffs;
               std::stringstream thisObj;
               if(numInStream == 0) {
                    thisObj.str( objStream.readsome(objStream.length()-firstObjByteOffs) );
               }
               else {
                   // objectPositions contains alternately objects numbers and their byte offsets
                   // in the object stream.
                   int counter = 0, ii = 0;
                   while(counter<numInStream*2+1) {
                       ii = objPositions.find_first_of(' ', ii+1);
                       counter++;
                   } // while: go where desired byte offset is
                   objByteOffs = std::atoi( objPositions.substr(ii).c_str() );

                   // Discard unmeaningful data.
                   objStream.readsome((size_t)objByteOffs);

                   thisObj.str( objStream.readsome() );
               } // if obj is not the first in the stream

               return bPdf::extractObject(thisObj, std::string::npos, trim, ignoreStreams); 
        } // if objNum is in xrefSection scope

    } // for xrefSections

    std::cout << "Warning: request for compressed object which is not resolvable." << std::endl;
    return "";
} 
