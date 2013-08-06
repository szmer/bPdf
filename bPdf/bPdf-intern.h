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
} ;
