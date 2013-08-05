size_t bPdfIn::resolveIndirect(const std::string& reference, size_t& pos, size_t& beg) {

   size_t checker = bPdf::isRef( reference.substr(pos), pos );

   if(checker == -1)
       return 0;
   // else checker == position of the first char of reference in string

   beg = checker;

   std::string objNumStr = reference.substr(checker);
   int objNum = std::atoi( objNumStr.c_str() );       // 1st space and remainder
                                                      // are ignored

   return getObjPos(objNum);
}

// Wrappers for the main function.
size_t bPdfIn::resolveIndirect(const std::string& ref) {
   size_t pos = 0, beg;
   return resolveIndirect(ref, pos, beg);
}
size_t bPdfIn::resolveIndirect(const std::string& ref, size_t& pos) {
   size_t beg;
   return resolveIndirect(ref, pos, beg);
}
