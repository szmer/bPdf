void PdfOut::finish() {

    /*resolve references in pages and parent page nodes*/
	/*parents' bussiness: (resolve indirects)*/
    for(std::vector<PdfNode>::iterator node = pagesNodes.begin(); node != pagesNodes.end(); node++)
	resolve(*node);

    std::cout << "Finish: Nodes resolved.\n";

	/* pages */
    for(std::vector<PdfPage>::iterator page = pages.begin(); page != pages.end(); page++)  
	resolve(*page);

    const unsigned int bound = (objects.size())-1; // here end required indirect objs and various other
    { int i = 1;
    for(std::vector<PdfPage>::iterator pg = pages.begin(); pg != pages.end(); pg++, i++) {
	{ std::ostringstream num;
	num << i + pagesNodes.size() + bound;
	pagesNodes[(pg->inDictNum)].join("/Kids", num.str() + " 0 R "); }

	{ int kidsCount = 1 + std::atoi(pagesNodes[(pg->inDictNum)].dictionary["/Count"].c_str());
			// ^ pointless, optimize ^
	std::ostringstream num;
	num << kidsCount;
	pagesNodes[(pg->inDictNum)].set("/Count", num.str()); 
	}
    }
    }

    std::cout << "Finish: Pages resolved.\n";

	/* write page tree: */
    for(std::vector<PdfNode>::iterator node = pagesNodes.begin(); node != pagesNodes.end();
		node++) {
	// page catalog:
	if(node->get("/Count") == "0")
	    continue;

	std::ostringstream cat;
	cat << bound + pagesNodes.size() + pages.size() + 1;
	node->set("/Parent", cat.str() + " 0 R");
	node->join("/Kids", "]");

	write(*node);
    }

    for(std::vector<PdfPage>::iterator pg = pages.begin(); pg != pages.end(); pg++) {
	
	// give parent entry:
	std::ostringstream posStr;
	posStr << pg->inDictNum + 1 + bound;
	pg->set("/Parent", posStr.str() + " 0 R"); 

	write(*pg);
    }

	/* write root of page tree:*/
    {
	objects.push_back(counter);
	std::ostringstream intStr;
	intStr << objects.size()-1;
	std::string root = intStr.str() + " 0 obj\n<< /Type /Pages\n/Kids [ ";
	for(int i = 1; i <= pagesNodes.size(); i++) {
	    std::ostringstream nodNum;
	    nodNum << (i + bound);
	    root += nodNum.str() + " 0 R ";
	}
	intStr.clear();
	intStr.str("");
	intStr << pagesNodes.size();
	root += "]\n/Count " + intStr.str() + " >>\nendobj\n";
	write(root);
    }

       /* write document catalog*/
    int docCat = objects.size();
    if(root.dictionary.size() == 0) {
	objects.push_back(counter);
	std::ostringstream intStr;
	intStr << objects.size()-1;
	std::string cat = intStr.str();
	intStr.clear();
	intStr.str("");
	intStr << objects.size()-2;// page tree root
	cat += " 0 obj\n<< /Type /Catalog\n/Pages "+intStr.str()+" 0 R >>\nendobj\n";
	write(cat);
    }
    else {
	root.set("/Pages", "");

 	if(root.source != 0) resolve(root);

	std::ostringstream intStr;
	intStr << objects.size()-1;// page tree root
	root.set("/Pages", intStr.str() + " 0 R");

	objects.push_back(counter);
	intStr.clear();
	intStr.str("");
	intStr << objects.size()-1;
	std::string cat = intStr.str() + " 0 obj\n" + Pdf::rollDict(root.dictionary) + "\nendobj\n";
	write(cat);
    }

	/* write xref table: */
    unsigned int xrefPos = counter;
    {
	std::ostringstream count;
	count << objects.size();
	write("xref\n0 " + count.str() + "\n0000000000 65635 f\n");
	for(int i = 1; i < objects.size(); i++) { // insert entries with byte offsets
	    std::ostringstream offs;
	    offs << objects[i];

	    for(int j = 0; j<(10-offs.str().length()); j++) write("0");
	
	    write(offs.str() + " 00000 n\n");
	}
	
    }
	/* write file trailer*/
    { 
	std::ostringstream strInt;
	strInt << objects.size();
	write("trailer\n<< /Size " + strInt.str() + "\n/Root ");
	strInt.clear();
	strInt.str("");
	strInt << docCat; // document catalog
	write(strInt.str() + " 0 R >>\n");
    }

    // end document:
    {
    	std::ostringstream strInt;
    	strInt << xrefPos;
	write("startxref\n" + strInt.str() + "\n%%EOF");
    }
}
