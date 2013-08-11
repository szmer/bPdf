int bPdfIn::resolveIndirect(const std::string& reference,
size_t& pos, size_t& beg, bool retry) {

   size_t rltvPos;
   size_t checker = bPdf::isRef( reference.substr(pos), rltvPos, retry );

   if(checker == -1)
       return -1;
   // else checker == position of the first char of reference in substring

   beg = checker+pos;
   pos += rltvPos;

   int objNum = std::atoi( &((reference.c_str())[beg]) ); // 1st space and remainder
                                                         // are ignored
   return objNum;
}

// Wrappers for the main function.
int bPdfIn::resolveIndirect(const std::string& ref, bool retry) {
   size_t pos = 0, beg;
   return resolveIndirect(ref, pos, beg, retry);
}

int bPdfIn::resolveIndirect(const std::string& ref, size_t& pos, bool retry) {
   size_t beg;
   return resolveIndirect(ref, pos, beg, retry);
}
