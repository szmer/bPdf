size_t bPdf::isArr(const std::string &str, size_t &end, bool retry) {
   if(str.size() == 0)
        return -1;

   size_t pos = str.find_first_of('[');

   if(pos == std::string::npos || (!retry && pos > 0))
        return -1;

   int lev = 0;     // level of embedding
   size_t pnt = 0;
   while(true) {
        size_t open = str.find_first_of('[', pnt);
        size_t close = str.find_first_of(']', pnt);
        if(close == std::string::npos) {
              end = -1;
              return pos;
        } // if close is nothing
        if(open != std::string::npos && open < close) {
              lev++;
              pnt = open;
              continue;
        }
        lev--;
        if(lev >= 0) {
              end = close;
              return pos;
        }
   } // while true

   return -1;
}
size_t bPdf::isArr(const std::string& str, bool retry) {
   size_t end;
   return isArr(str, end, retry);
}

size_t bPdf::isDict(const std::string &str, size_t &end, bool retry) {
   if(str.size() == 0)
        return -1;

   size_t pos = str.find("<<");

   if(pos == std::string::npos || (!retry && pos > 0))
        return -1;

   int lev = 0;     // level of embedding
   size_t pnt = 0;
   while(true) {
        size_t open = str.find("<<", pnt);
        size_t close = str.find(">>", pnt);
        if(close == std::string::npos) {
              end = -1;
              return pos;
        } // if close is nothing
        if(open != std::string::npos && open < close) {
              lev++;
              pnt = open;
              continue;
        }
        lev--;
        if(lev >= 0) {
              end = close;
              return pos;
        }
   } // while true

   return -1;
}
size_t bPdf::isDict(const std::string& str, bool retry) {
   size_t end;
   return isDict(str, end, retry);
}

size_t bPdf::isRef(const std::string &str, size_t &end, bool retry) {

   if(str.length() < 5)
	return -1;

   for(size_t i=str.find_first_of("0123456789");
       i<(int)(str.length()-4) && i != std::string::npos; ) {

      if(!retry && i != 0)
          return -1;

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

size_t bPdf::isRef(const std::string& str, bool retry) {
   size_t end;
   return isRef(str, end, retry);
}
