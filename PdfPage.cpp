PdfPage::PdfPage(std::string ref, int number, std::map<std::string, std::string> indict, PdfIn* gdoc) {
    reference = ref;
    inheritedDict = indict;
    source = gdoc;
    contentsPnt = 0;

    // load dict
    source->file.seekg(static_cast<long int>(source->resolveIndirect(reference)) , std::ios::beg);
    std::string td = source->extractObject();
    dictionary = Pdf::unrollDict(td);
    set("/Parent", "");
}

PdfStream PdfPage::nextContentStream() {
    std::string stack = get("/Contents").substr(contentsPnt);
    std::cout << "stack: " << stack << "\n";
    unsigned int byteOffset = source->resolveIndirect(stack, contentsPnt, contentsPnt);

    source->file.seekg(byteOffset, std::ios::beg);
    std::string td = source->extractObject(true); // leave alone the stream
    std::cout << "extracted object:\n" << td << "\n";
    return PdfStream(Pdf::unrollDict(td), source->file.tellg(), source);
}

bool PdfPage::endOfStreams() {
    int check = contentsPnt;
    if(source->resolveIndirect(get("/Contents").substr(contentsPnt), check, check) == 0)
	return true;
    else
	return false;
}
