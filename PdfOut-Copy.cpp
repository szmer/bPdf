int PdfOut::copy(PdfIn *doc, unsigned int byteOffset) {

   for(int i = 0; i < objCopied.size(); i++) 
	if(byteOffset == objCopied[i]) return i+1;

   std::string object;
   size_t strm;
   size_t endobj;

   { std::string buffer;
     size_t docpos = byteOffset;
   /* resolve indirects: */
   while(true) {
  	doc->file.seekg(docpos, std::ios::beg);
	std::getline(doc->file, buffer);
	docpos = doc->file.tellg(); // must be saved because we will move pointer while resolving refs

        strm = buffer.find("stream");
	endobj = buffer.find("endobj");

	// looking for references
	std::string innerBuffer;
	if(strm != std::string::npos) {
	   innerBuffer = buffer.substr(0, strm);
	   buffer.erase(0,strm);
	}
	else {
	   innerBuffer = buffer.substr(0, endobj); // regardless of actual value of endobj (npos will be fine)
	   buffer.erase(0,endobj);
	}
   	std::string newInnerBuff = "";
	int lpos = 0;
	while(true) {
	    int pos = lpos;
	    int beg = 0;
	    unsigned int subOffset = doc->resolveIndirect(innerBuffer, pos, beg);
	    if(subOffset > 0) {
		int ref = copy(doc, subOffset);
	    	std::ostringstream strRef;
	    	strRef << ref << " 0 R";
		newInnerBuff += innerBuffer.substr(lpos,beg-lpos) + strRef.str();
		lpos = pos+1;
	    }
	    else {
		newInnerBuff += innerBuffer.substr(lpos);
		break;
	    }
	}
	object += newInnerBuff + buffer + '\n';

	if(strm != std::string::npos || endobj != std::string::npos) 
	   break; 
   } } // line by line
   
   for(int i = 0; i < objCopied.size(); i++) 
	if(byteOffset == objCopied[i]) return i+1;

   objCopied.push_back(byteOffset);

   /*update object header:*/
   objects.push_back(counter);
   int thisRef = objects.size() - 1;
   {
   size_t obj = object.find("obj");
   std::ostringstream thisRefStr;
   thisRefStr << thisRef;
   object = thisRefStr.str() + " 0 " + object.substr(obj);
   }

   // update:
   strm = object.find("stream");
   endobj = object.find("endobj");

   if(endobj != std::string::npos) {
	write(object.substr(0, endobj) + "endobj\n");
	return thisRef;
   }

   write(object);

   if(strm != std::string::npos) {
	while(true) {
	    std::string buffer;
	    std::getline(doc->file, buffer);
	    endobj = buffer.find("endobj");

	    if(endobj != std::string::npos) {
		buffer = buffer.substr(0, endobj);
		write(buffer + "endobj\n");
		return thisRef;	
	    }
	    write(buffer + '\n');
   	}
   }
   return thisRef;
}
