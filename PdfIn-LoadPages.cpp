PdfPageCat PdfIn::loadPages() {

	PdfPageCat catalog;

	/* loading root catalog */
	if(trailer.count("/Root") < 1) {
		std::cout << "Loading pages: root catalog cannot be found.\nTRAILER\n";
		for(std::map<std::string,std::string>::iterator i=trailer.begin(); i!=trailer.end(); i++)
		    std::cout << i->first << " => " << i->second << '\n';
		return catalog;
	}

	std::string source = extractObject(true,resolveIndirect(trailer["/Root"]));
	catalog.root = unrollDict(source);

	/* passing through page tree */
	if(catalog.root.count("/Pages") < 1) {
		std::cout << "There are no page tree.\n";
		return catalog;
	}

	// load root node of the tree:
	file.seekg(static_cast<long int>(resolveIndirect(catalog.root["/Pages"])), std::ios::beg);
	source = extractObject();
	PdfInPages root;
	root.attributes = unrollDict(source);

	// reading stack - currently processed one in the surface:
	std::vector<PdfInPages> tree;

	// initialize queue:
	if(root.attributes.count("/Kids") < 1) {
		std::cout << "Page tree root has no kids.\n";
		return catalog;
	}
	root.kids = unrollArray(root.attributes["/Kids"]);
	tree.push_back(root);

	do {
		if(tree.back().kids.size() > 0) { // move to the first child (which means "last" - vector is reversed) and remove it
			file.seekg(static_cast<long int>(resolveIndirect( tree.back().kids.back() )), std::ios::beg);
			// kidsToRead.pop_back() in the end of loop
		}
		else { // no kids left - move up the tree
			tree.pop_back();
			continue;
		}

		std::string td = extractObject();
		std::map<std::string, std::string> identify = unrollDict(td);

		if(identify.count("/Type") < 1) {
			break;
		}

		/* if it is a leaf (page)... */
		if(identify["/Type"] == "/Page") {
			PdfPageCat::Entry page;
			page.reference = tree.back().kids.back();

			if(tree.back().attributesIndex > 0)
				page.attributesDict = tree.back().attributesIndex;
			else { // build new attributes dictionary
				std::map<std::string, std::string> attributes;
				attributes.insert(tree.back().attributes.begin(), tree.back().attributes.end());
				// merge dictionaries (inheritance), keeping values of closer to page ones:
				for(int i = 0; i < (tree.size() - 1); i++)
					attributes.insert(tree[i].attributes.begin(), tree[i].attributes.end());
				catalog.attributesDicts.push_back(attributes);
				tree.back().attributesIndex = static_cast<unsigned int>(catalog.attributesDicts.size() - 1);
				page.attributesDict = static_cast<unsigned int>(catalog.attributesDicts.size() - 1);
			}

			catalog.pages.push_back(page);
			/* move on in the queue */
			tree.back().kids.pop_back();
			continue;
		} // "/Page"

		/* else... (new node) */	
		PdfInPages node;
		node.attributes = unrollDict(td);

		if(node.attributes.count("/Kids") < 1) {
			break;
		}
		node.kids = unrollArray(node.attributes["/Kids"]);
		std::reverse(node.kids.begin(), node.kids.end()); // necessary to keep pages in order when reading

		/* move on in the queue */
		tree.back().kids.pop_back();
		tree.push_back(node);

	} while(tree.size() != 0);

	catalog.doc = this;
	return catalog;
}
