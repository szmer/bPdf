PdfPage PdfPageCat::getPage(int number) {

	if(!(number < pages.size())) {
		PDFIN_FATAL("Demanded page is out of catalog bounds. Program terminated. \r\n");
		exit(1);
	}
	PdfPage page(pages[number].reference, number, attributesDicts[pages[number].attributesDict], this->doc);
	
	return page;
}

