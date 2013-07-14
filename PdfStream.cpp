PdfStream::PdfStream(std::map<std::string, std::string> dict, size_t pos, PdfIn* src) {
   streamPnt = pos;
   source = src;
   dictionary = dict;
}

std::string PdfStream::fetchWholeStream() {
   return source->extractObject(false, streamPnt);
}
