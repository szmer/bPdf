std::string bPdfIn::extractStream(size_t length, size_t pos) {

    if(pos != 0)
        file.seekg(pos, std::ios::beg);

    std::string stream;
    stream.reserve(length);
    for(size_t i = 0; i<length; i++)
	stream += file.get();

    return stream;
}
