struct bPdfXrefSection {
    int start;
    int end;
    int entryLength;

    bool isCompressed;

    // Uncompressed tables.
    size_t pos;
    
    // Compressed tables.
    std::string data;                   // decompressed
    std::vector<int> entryStructure;    // W array in xref stream dict
    // Dictionary of most recent xref stream is saved as trailer.

    // This function works only with xref streams (compressed tables).
    size_t comprLookup(int objNum, int& info);
    // Return value and info variable are both important, see below.
    // ---------------------------------------------------------
    //                    |  return value   |   info variable
    // uncompressed table |       -1        |        -1
    // type 0 entry (free)|        0        | next free obj num
    // type 1 entry (used)| obj byte offset |        -1
    // type 2 entry (stre-|  number of the  |   obj num in the
    //               amed)|  stream object  |      stream
    //    broken entry    |       -2        |         -2
    // ---------------------------------------------------------
} ;
