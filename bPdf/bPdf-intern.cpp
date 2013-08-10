size_t bPdfXrefSection::comprLookup(int objNum, int& info) {

    if(!isCompressed) {
        info = -1;
        return -1; 
    }
    if(entryStructure.size() != 3 && entryStructure.size() != 2) 
        throw "Invalid W table of compressed xref table.";

    objNum -= start;         // adjust to local "space" for this section
    std::string entry;

    if(objNum == 0)
        entry = data.substr(0, entryLength);
    else {
        int dataPnt = entryLength * objNum;
        entry = data.substr(dataPnt, entryLength);
    }

    // Compute value of the second field which is most meaningful.
    int sndField = 0;
    for(int i=entryStructure[0]; i<entryStructure[0]+entryStructure[1]; i++)
         sndField = sndField << 8 | (unsigned char)entry[i];

    // Finish, depending on first field's content.
    if(entryStructure[0] == 0) { // all entries default to type 1 (normal)
         info = -1;
         return sndField;
    }

    // Compute value of the first field (entry type).
    int entryType = 0;
    for(int i=0; i<entryStructure[0]; i++)
         entryType = entryType << 8 | (unsigned char)entry[i];

    switch(entryType) {
        case 0:
         info = sndField;
         return 0;
        case 1: 
         info = -1;
         return sndField;
        case 2:
         info = 0;
         for(int i = entryLength-entryStructure[2]; i<entryLength; i++)
           info = info << 8 | (unsigned char)entry[i];
         return sndField;  
    }
    info = -2;
    return -2;
}
