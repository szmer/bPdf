struct bPdfXrefSection {
    int start;
    int end;
    size_t pos;

    bool isCompressed;

    // Uncompressed tables.
    int entryLength;
    
    // Compressed tables.
    std::string data;                   // decompressed
    std::vector<int> entryStructure;    // W array in xref stream dict
    // Dictionary of most recent xref stream is saved as trailer.
} ;
