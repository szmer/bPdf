std::string PdfNode::get(std::string key) {
   if(dictionary.count(key) > 0)
	return dictionary[key];
   return "";
}

void PdfNode::set(std::string key, std::string value) {
    dictionary[key] = value;
}
void PdfNode::join(std::string key, std::string value) {
    dictionary[key] += value;
}
