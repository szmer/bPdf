void PdfOut::add(PdfPage page) {
    //replace dictionary with "pointer" to its copy:
    for(std::vector<PdfNode>::iterator node = pagesNodes.begin();; node++) {
	if(node == pagesNodes.end()) {
	   page.inDictNum = pagesNodes.size();

	   PdfNode newNode;
	   newNode.source = page.source;
	   newNode.dictionary = page.inheritedDict;
	   newNode.set("/Kids", "[ ]");
	   newNode.set("/Parent", "0");
	   newNode.set("/Count", "0");
	   pagesNodes.push_back(newNode);

	   page.inheritedDict.clear();
	   break;
	}
	if(node->dictionary == page.inheritedDict && node->source == page.source) {
	   page.inDictNum = std::distance(pagesNodes.begin(), node);
	   page.inheritedDict.clear(); 
	}
    }
	
    pages.push_back(page);
    return;
}
void PdfOut::write(std::string str) {
    file.write(str.c_str(),(str.length()));
    counter += str.length();
    return;
}
/*// resolve references in dict
void PdfOut::resolve(PdfNode &node) {
   for(std::map<std::string, std::string>::iterator i = node.dictionary.begin(); 
           i != node.dictionary.end(); i++)
   {
   	std::string newEntry = "";
	int lpos = 0;
	while(true) {
	    int pos = lpos;
	    int beg = 0;
	    unsigned int byteOffset = node.source->resolveIndirect(i->second, pos, beg);
	    if(byteOffset > 0) {
		int ref = copy(node.source, byteOffset);
	    	std::ostringstream strRef;
	    	strRef << ref << " 0 R";
		newEntry += i->second.substr(lpos,beg-lpos) + strRef.str();
		lpos = pos+1;
	    }
	    else {
		newEntry += i->second.substr(lpos);
		break;
	    }
	}
   	i->second = newEntry;
   } // passing through std::map
}

std::map<int,int> PdfOut::findRefs(std::string &string, PdfIn* source) {
   std::map<int,int> positions;
   int pos = 0;
   do {
      int beg = 0;
      unsigned int check = source->resolveIndirect(string, pos, beg);
      if(check == 0) break;
      else
	positions[beg] = pos - beg + 1;
   } while(true);
   return positions;
}

int PdfOut::write(PdfNode &node) {
    objects.push_back(counter);
    int ref = objects.size() - 1;
    std::string writeDict = rollDict(node.dictionary);
    { std::ostringstream strRef;
    strRef << ref;
    writeDict = strRef.str() + " 0 obj\n" + writeDict + "\nendobj\n";
    }    write(writeDict);
    return ref;
}
*/
void PdfOut::insertRoot(std::map<std::string, std::string> givenRoot) {
    root.dictionary = givenRoot;
    root.source = 0;
    return;
}
void PdfOut::insertRoot(PdfPageCat givenObj) {
    root.dictionary = givenObj.root;
    root.source = givenObj.doc;
    return;
}
PdfOut::PdfOut(const char* filename) {
    file.open(filename, std::fstream::out | std::ofstream::trunc);

    if(file.good() == false)
	std::cout << "Opening output Pdf file " << filename << " failed.\n";

    positions.push_back(0); // this will be "free" object in xref table
    counter = 0;

    // write PDF header to file:
    std::string header = "%PDF-1.7\n%âãÏÓ\n";
    write(header);
    return;
}
