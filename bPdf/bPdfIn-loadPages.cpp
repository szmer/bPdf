void bPdfIn::loadPages() {

    // Load root 
    if(trailer.count("/Root") < 1)
	throw "Root cannot be found.";

    root = bPdf::unrollDict( getObjByNum(resolveIndirect(trailer["/Root"])) );

    // Load page tree root node.
    if(root.count("/Pages") < 1)
	throw "PDF file contains no pages.";

    bPdfNode rootNode;
    rootNode.objNum = resolveIndirect(root["/Pages"]);
    rootNode.dict = bPdf::unrollDict( getObjByNum(rootNode.objNum) );
    if(rootNode.dict.count("/Kids") == 0)
	throw "Page tree root node has no kids.";

    // Load the whole structure into 
    // Here is two vectors which will be used simultuneously to iterate.
    std::vector<bPdfNode> nodesStack;
    std::vector<size_t> kidsCollectPos;	  // pos variables used to iterate with resolveIndirect
				 	  // and find kids' objects

    nodesStack.push_back(rootNode);
    kidsCollectPos.push_back(0);

    while(nodesStack.size() > 0) {
	size_t nodeObjNum = resolveIndirect(nodesStack.back().get("/Kids"), kidsCollectPos.back());

	if(nodeObjNum != -1) {
	     dictionary nodeDict = bPdf::unrollDict( getObjByNum(nodeObjNum) );

	     if(nodeDict.count("/Type") == 0) {
		continue;
	     } // if there is no type

	     // Construct page object and add it to the 
	     else if(nodeDict["/Type"] == "/Page") {
		bPdfPage page;
		page.source = this;
		page.objNum = nodeObjNum;
		page.dict = nodeDict;

		if(   !inheritedDicts.empty() &&
                        (inheritedDicts.back())["reference"] == page.get("/Parent")   )
		    page.inherDictNum = inheritedDicts.size()-1;

		else {
		// Make one "inherited dictionary" containinig all entries defined on higher
		// levels of the structure. Entries from lower level will always be preserved.
		   dictionary inherDict;
		   for(int i=nodesStack.size()-1; i!=0; i--)
		       { inherDict.insert(nodesStack[i].dict.begin(),
					nodesStack[i].dict.end()); }
                   inherDict["reference"] = page.get("/Parent");
		   inheritedDicts.push_back(inherDict);
		   page.inherDictNum = inheritedDicts.size()-1;
		} // else (make inherited dict)

		pages.push_back(page);
		continue;
	     } // if page

	     // If not page, add new item to the stack.
	     bPdfNode node;
	     node.dict = nodeDict;
	     nodesStack.push_back(node);
	     kidsCollectPos.push_back(0);
	} // if kid is found
	else {
	     nodesStack.pop_back();
	     kidsCollectPos.pop_back();
	} // no kids left, move up
    } // while (consume stack of page tree nodes)

    return;
}
