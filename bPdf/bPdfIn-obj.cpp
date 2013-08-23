std::string bPdfIn::obj(const std::string& str) {
    size_t objnum = resolveIndirect(str);
    if(objnum == -1)
         return str;
    return getObjByNum(objnum);
}
