std::string bPdfPage::get(std::string) {
   if(dict.count(key) > 0)
       return dict[key];
   else if(inherDict->count(key) > 0)
       return (*inherDict)[key];
   return "";
}
