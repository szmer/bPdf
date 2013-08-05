size_t bPdf::isRef(const std::string &str, size_t &end) {
   if(str.length() < 5)
	return 0;

   for(int i=0; i<(int)(str.length()-4); i++) {

      // Try to examine the first number of the reference.
      size_t firstSpc = str.substr(i).find_first_of(" ");
      // Note: firstSpc is RELATIVE to str.substr(i).

      if(firstSpc == std::string::npos || firstSpc+1 == str.length()) // there cannot be
          return -1;                                                  // ref without spaces
      if(firstSpc == 0)
          continue;

      if(str.substr(i,firstSpc).find_first_not_of("0123456789") != std::string::npos) // only
          continue;                                                                  // digits?


      // Try to examine the second number of the reference.
      size_t nextSpc = str.substr(i+firstSpc+1).find_first_of(" ") + firstSpc+1;
      // Note: nextSpc is RELATIVE to substr(i) - as firstSpc.

      if(nextSpc == std::string::npos || nextSpc+1 == str.length())
           return -1;

      if((str.substr(i))[nextSpc+1] != 'R'
        || str.substr(i+firstSpc+1, nextSpc-firstSpc-1).find_first_not_of("0123456789") != std::string::npos) {
      // (the second condition checks sanity of generation number)
           i = nextSpc;
           continue;
      }


      // If we got there, it means that we really found an indirect reference.
      end = i+nextSpc+1;     // 'R'
      return i; 
   }
   return -1;
}

size_t bPdf::isRef(const std::string& str) {
   size_t end;
   return isRef(str, end);
}
