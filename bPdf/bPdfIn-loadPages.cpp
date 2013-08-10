bPdfPageCat bPdfIn::loadPages() {

    bPdfPageCat catalog;

    // Load root catalog.
    if(trailer.count("/Root") < 1)
	throw "Root catalog cannot be found.";

    catalog.root = bPdf::unrollDict( getObjByNum(resolveIndirect(trailer["/Root"])) );

    // Load page tree root node.
    if(catalog.root.count("/Pages") < 1)
	throw "PDF file contains no pages.";

    bPdfNode rootNode;
    rootNode.objNum = resolveIndirect(catalog.root["/Pages"]);
    rootNode.dict = bPdf::unrollDict( getObjByNum(rootNode.objNum) );
    if(rootNode.dict.count("/Kids") == 0)
	throw "Page tree root node has no kids.";

    // Load the whole structure into catalog.
    // Here is two vectors which will be used simultuneously to iterate.
    std::vector<bPdfNode> nodesStack;
    std::vector<size_t> kidsCollectPos;	  // pos variables used to iterate with resolveIndirect
				 	  // and find kids' objects
    std::vector<int> dictNums;    // positive number is index in catalog's inheritedDicts vector,
				     // -1 means such dictionary must be generated

    nodesStack.push_back(rootNode);
    kidsCollectPos.push_back(0);
    dictNums.push_back(-1);

    while(nodesStack.size() > 0) {
	size_t nodeObjNum = resolveIndirect(nodesStack.back().get("/Kids"), kidsCollectPos.back());

	if(nodeObjNum != -1) {
	     dictionary nodeDict = bPdf::unrollDict( getObjByNum(nodeObjNum) );

	     if(nodeDict.count("/Type") == 0) {
		continue;
	     } // if there is no type

	     // Construct page object and add it to the catalog.
	     else if(nodeDict["/Type"] == "/Page") {
		bPdfPage page;
		page.source = this;
		page.objNum = nodeObjNum;
		page.dict = nodeDict;

		if(dictNums.back() >= 0)
		    page.inherDictNum = dictNums.back();

		else {
		// Make one "inherited dictionary" containinig all entries defined on higher
		// levels of the structure. Entries from lower level will always be preserved.
		   dictionary inherDict;
		   for(int i=nodesStack.size()-1; i!=0; i--)
		       { inherDict.insert(nodesStack[i].dict.begin(),
					nodesStack[i].dict.end()); }
		   catalog.inheritedDicts.push_back(inherDict);
		   page.inherDictNum = catalog.inheritedDicts.size()-1;
		   dictNums.back() = page.inherDictNum;
		} // else (make inherited dict and add to catalog)

		catalog.pages.push_back(page);
		continue;
	     } // if page

	     // If not page, add new item to the stack.
	     bPdfNode node;
	     node.dict = nodeDict;
	     nodesStack.push_back(node);
	     kidsCollectPos.push_back(0);
	     dictNums.push_back(-1);
	} // if kid is found
	else {
	     nodesStack.pop_back();
	     kidsCollectPos.pop_back();
	     dictNums.pop_back();
	} // no kids left, move up
    } // while (consume stack of page tree nodes)

    catalog.doc = this;
    return catalog;
}
