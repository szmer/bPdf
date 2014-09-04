#include "bPdf-structure.h"

std::string bPdfPage::get(const char* key) {
   if(dict.count(key) > 0)
       return dict[key];
   else if(source->inheritedDicts[inherDictNum].count(key) > 0)
       return source->inheritedDicts[inherDictNum][key];
   return "";
}
