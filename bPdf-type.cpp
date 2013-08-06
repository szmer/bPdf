size_t bPdf::isRef(const std::string &str, size_t &end) {

   if(str.length() < 5)
	return -1;

   for(size_t i=str.find_first_of("0123456789");
       i<(int)(str.length()-4) && i != std::string::npos; ) {

      // Try to examine the first number of the reference.
      size_t firstSpc = str.find_first_of(" ", i);

      if(firstSpc == std::string::npos || firstSpc+1 == str.length()) // there cannot be
          return -1;                                                  // ref without spaces
      if(firstSpc == 0)
          continue;

      if(str.substr(0, firstSpc).find_first_not_of("0123456789", i) != std::string::npos){
                                                                         // only digits?
          i = str.find_first_of("0123456789", firstSpc);
          continue;
      }

      // Try to examine the second number of the reference.
      size_t nextSpc = str.find_first_of(" ", firstSpc+1);

      if(nextSpc == std::string::npos || nextSpc+1 == str.length())
           return -1;

      if(str[nextSpc+1] != 'R'
        || str.substr(0,nextSpc).find_first_not_of("0123456789", firstSpc+1) != std::string::npos) {
      // (the second condition checks sanity of generation number)
           i = str.find_first_of("0123456789", nextSpc);
           continue;
      }


      // If we got there, it means that we really found an indirect reference.
      end = nextSpc+1;     // 'R'
      return i; 
   }
   return -1;
}

size_t bPdf::isRef(const std::string& str) {
   size_t end;
   return isRef(str, end);
}
