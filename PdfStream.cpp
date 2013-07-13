PdfStream::PdfStream(std::map<std::string, std::string> dict, size_t pos, PdfIn* src) {
   currentPnt = pos;
   source = src;
   dictionary = dict;
}

std::string PdfStream::getline() {
   source->file.seekg(currentPnt, std::ios::beg);
   std::string line;
   std::getline(source->file, line);
   currentPnt = source->file.tellg();
   return line;
}

bool PdfStream::end() {
   return !(source->file.good());
}
