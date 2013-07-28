PdfPage PdfPageCat::getPage(int number) {

	if(!(number < pages.size())) {
	     throw "Demanded page is out of catalog bounds. Program terminated. \r\n";
	}
	PdfPage page(pages[number].reference, number, attributesDicts[pages[number].attributesDict], this->doc);
	
	return page;
}

