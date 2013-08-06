size_t bPdfIn::resolveIndirect(const std::string& reference, size_t& pos, size_t& beg) {

   size_t rltvPos;
   size_t checker = bPdf::isRef( reference.substr(pos), rltvPos );

   if(checker == -1)
       return 0;
   // else checker == position of the first char of reference in substring

   beg = checker+pos;
   pos += rltvPos;

   int objNum = std::atoi( &((reference.c_str())[beg]) ); // 1st space and remainder
                                                         // are ignored
   (2 == 2);
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