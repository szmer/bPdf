void PdfOut::finish() {

    /*** resolve references in pages and parent page nodes ***/
    for(int i=0; i<pages.size(); i++) 
	pages[i].dictionary = Pdf::unrollDict(
			copyResources(*(pages[i].source), pages[i].dictionary) );

    for(int i=0; i<pagesNodes.size(); i++) {
	pagesNodes[i].dictionary = Pdf::unrollDict(
			copyResources(*(pagesNodes[i].source), pagesNodes[i].dictionary) );
	pagesNodes[i].set("/Kids", "[ ");
    }

    /*** write pages and page nodes ***/
    const int bound = positions.size(); // here end all indirect objects except page tree
    const int pagesBound = bound + pages.size();
    std::string pageTreeRootReference = Pdf::itoa(pagesBound + pagesNodes.size()) + " 0 R";

    /* pages */
    for(int i=0; i<pages.size(); i++) {

	// add reference to this page in the proper node (opening '[' was already placed above)
	pagesNodes[ pages[i].inDictNum ].join("/Kids", Pdf::itoa(i + bound) + " 0 R ");

	// add reference to the parent node
	pages[i].set("/Parent", Pdf::itoa( pages[i].inDictNum+pagesBound ) + " 0 R"); 

	writeIndirectObject(Pdf::rollDict(pages[i].dictionary));
    }

    /* pages nodes */
    std::string pageTreeRoot = "<< /Type /Pages\n/Kids [ ";

    for(int i=0; i<pagesNodes.size(); i++) {

	pagesNodes[i].join("/Kids", "]");
	pagesNodes[i].set("/Parent", pageTreeRootReference);

	// count the kids:
	std::string kidsStr = pagesNodes[i].get("/Kids");
	int counter = 0;
	for(int j=0; j < kidsStr.length(); j++)
	    if(kidsStr[j] == 'R')
		counter++;
	pagesNodes[i].set("/Count", Pdf::itoa(counter));

	int nodeObjNum = writeIndirectObject( Pdf::rollDict(pagesNodes[i].dictionary) );
	pageTreeRoot += Pdf::itoa( nodeObjNum ) + " 0 R ";	
    }

    pageTreeRoot += "]\n/Count " + Pdf::itoa( pagesNodes.size() ) + " >>";
    writeIndirectObject(pageTreeRoot);

    /* document root catalog */
    if(root.dictionary.size() == 0) {
        std::string documentCatalog = "<< /Type /Catalog\n/Pages " + pageTreeRootReference + " >>";
        writeIndirectObject(documentCatalog);
    }
    else {
	std::string rootDictionary;
	if(root.source != 0)
	     rootDictionary = copyResources(*(root.source), Pdf::rollDict(root.dictionary));
	else
	     rootDictionary = Pdf::rollDict(root.dictionary);
	writeIndirectObject(rootDictionary);
    }

    /*** xref table: ***/
    std::string documentSize = Pdf::itoa( positions.size() );
    unsigned int xrefPos = counter;

    std::string xrefTable = "xref\n0 " + documentSize + " \n0000000000 65635 f\n";
    for(int i=1; i<positions.size(); i++) {
	std::string byteOffset = Pdf::itoa(positions[i]);
	for(int j=byteOffset.size(); j<10; j++) // byte offset must contain 10 bytes
	     xrefTable += "0";
	xrefTable += byteOffset;
	xrefTable +=" 00000 n \n";
    }
    write(xrefTable);

    /*** file trailer ***/
    write("trailer\n<< /Size " + documentSize + "\n/Root "
		+ Pdf::itoa( positions.size()-1 ) + " 0 R >>\n");

    /*** end document ***/
    write("startxref\n" + Pdf::itoa(xrefPos) + "\n%%EOF");
}
