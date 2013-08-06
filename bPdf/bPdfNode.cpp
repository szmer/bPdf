std::string bPdfNode::get(std::string key) {
   if(dict.count(key) > 0)
	return dict[key];
   return "";
}

void bPdfNode::set(std::string key, std::string value) {
    dict[key] = value;
}
void bPdfNode::join(std::string key, std::string value) {
    dict[key] += value;
}
