std::string bPdfNode::get(const char* key) {
   if(dict.count(key) > 0)
	return dict[key];
   return "";
}

void bPdfNode::set(const char* key, const char* value) {
    dict[key] = value;
}
void bPdfNode::join(const char* key, const char* value) {
    dict[key] += value;
}
