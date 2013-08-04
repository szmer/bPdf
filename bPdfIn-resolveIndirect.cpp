size_t bPdfIn::resolveIndirect(const std::string& reference, int& pos, int& beg) {

   // Check if it is possible to find an indirect reference here.
   if(reference.length()-pos < 5)
	return 0;

   const std::string ref = reference.substr(pos);
   size_t objNum = 0;                                   // to be extracted

   // Test possible positions where reference may begin.
   // Format is "<number> <number> R". First number is byte offset of the object in file.
   for(int i=0; i<(int)(ref.length()-4); i++) {

      // Try to examine the first number of the reference.
      size_t firstSpc = ref.substr(i).find_first_of(" ");
      if(firstSpc == std::string::npos || firstSpc+1 == ref.length()) // there cannot be
          return 0;                                                  //  indirect without spaces
      if(firstSpc == 0)
          continue;

      std::string objNumStr = ref.substr(i,firstSpc);
      if(objNumStr.find_first_not_of("0123456789") != std::string::npos)
          continue;

      objNum = std::atoi(objNumStr.c_str());

      // Try to examine the second number of the reference.
      size_t nextSpc = ref.substr(i+firstSpc+1).find_first_of(" ") + firstSpc+1;
      if(nextSpc == std::string::npos || nextSpc+1 == ref.length())
           return 0;

      if((ref.substr(i))[nextSpc+1] != 'R'
        || ref.substr(i+firstSpc+1, nextSpc-firstSpc-1).find_first_not_of("0123456789") != std::string::npos) {
           i = nextSpc;
           continue;
      }

      // If we got there, it means that we really found an indirect reference.
      beg = pos+i;
      pos = pos+i+nextSpc+2;     // after 'R'
      return getObjPos(objNum);
   } // for i to reference.length()-4

   return 0;
}

// Wrappers for the main function.
size_t bPdfIn::resolveIndirect(const std::string& ref) {
   int pos = 0, beg;
   return resolveIndirect(ref, pos, beg);
}
size_t bPdfIn::resolveIndirect(const std::string& ref, int& pos) {
   int beg;
   return resolveIndirect(ref, pos, beg);
}
