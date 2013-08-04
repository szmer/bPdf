bPdfPage bPdfPageCat::getPage(int number) {
   if(number >= (int)pages.size())
      throw "Trying to get page from out of bounds.";

   return pages[number];
}

