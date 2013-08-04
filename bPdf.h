#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#ifndef BPDF_H
#define BPDF_H

typedef std::map<std::string, std::string> dictionary;

// some structs for strictly internal purposes:
#include "bPdf-intern.h"

class bPdfIn;
struct bPdfPageCat;

struct bPdf {
    static dictionary unrollDict(const std::string&);
    static std::vector<std::string> unrollArray(const std::string&);

    static std::string rollDict(dictionary&);
    static std::string rollArray(const std::vector<std::string>&);

    static std::string extractObject(std::istream &source, size_t startPos = std::string::npos,
                                    bool ignoreStreams = false, bool trim = false);
    static std::string itoa(int);
} ;

struct bPdfNode {
    bPdfIn* source;

    std::string get(std::string);
    void set(std::string, std::string);
    void join(std::string, std::string);

    dictionary dict;
    size_t positionInSource;
} ;

struct bPdfPage : public bPdfNode {
// has dictionary and get-set-join functions
    int inherDictNum;
} ;

class bPdfIn {
  public:
    // initialization methods
    bPdfPageCat loadPages();

    // PDF objects methods
    size_t getObjPos(const int);
    size_t resolveIndirect(const std::string&);
    size_t resolveIndirect(const std::string&, int&);
    size_t resolveIndirect(const std::string&, int&, int&);

    std::string extractObject(size_t startPos = std::string::npos, bool ignoreStreams = false, bool trim = false);
    std::string extractStream(size_t length, size_t pos = 0);

    dictionary trailer;

    bPdfIn(const char*);
    ~bPdfIn();

  private:
    std::ifstream file;

    std::vector<bPdfXrefSection> xrefSections;
} ;

class bPdfPageCat {
  public:   
    bPdfPage getPage(int);
    int count() { return pages.size(); }

  private:
    bPdfIn *doc;
    std::vector<bPdfPage> pages;
    std::vector<dictionary> inheritedDicts;
    dictionary root;

  friend class bPdfIn;
} ;

#include "bPdf.cpp"
#include "bPdf-auxil.cpp" // itoa (integer to string)
#include "bPdf-extractObject_Stream.cpp"

#include "bPdfIn.cpp"
#include "bPdfIn-getObjPos.cpp"
#include "bPdfIn-resolveIndirect.cpp"
#include "bPdfIn-loadPages.cpp"

#include "bPdfNode.cpp"
#include "bPdfPageCat.cpp"

   #endif // BPDF_H
