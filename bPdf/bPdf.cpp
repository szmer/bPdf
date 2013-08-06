// unrollDict
dictionary bPdf::unrollDict(const std::string& str) {

    dictionary dict;
    std::stringstream strStream;
    if(str.find("<<") == std::string::npos)
         strStream.str(str.c_str());
    else
         strStream.str(str.substr(str.find("<<")+2).c_str());

    while(strStream.good()) {
	std::string key = bPdf::extractObject(strStream);	
	std::string value = bPdf::extractObject(strStream);
	if(key != "" && value != "")
	     dict.insert( std::pair<std::string, std::string>(key, value) );
    }

    return dict;
}

// unrollArray()
std::vector<std::string> bPdf::unrollArray(const std::string &str) {

    std::vector<std::string> array;
    std::stringstream strStream(str);

    while(strStream.fail() || strStream.get() != '[');
    if(strStream.fail())
        throw "Trying to unroll array from non-array string.";

    std::string elem;

    while((elem = bPdf::extractObject(strStream)) != ""
	&& strStream.good())
      { array.push_back(elem); }

    return array;
}

std::string bPdf::rollDict(dictionary &dictMap) {

	std::string dict = "<< ";
	for(dictionary::iterator i = dictMap.begin(); i != dictMap.end(); i++) 
		dict += ((*i).first + ' ' + (*i).second + '\n') + ' ';

	return (dict+" >>");
}

std::string bPdf::rollArray(const std::vector<std::string> &arrVect) {

	std::string array = "[ ";
	for(int i = 0; i < (int)arrVect.size(); i++)
		array += (arrVect[i] + ' ');

	return (array + " ]");
}
