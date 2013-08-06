#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#ifndef BPDF_H
#define BPDF_H

#include "bPdf-definitions.h"
// some structs for strictly internal purposes:
#include "bPdf-intern.h"

class bPdfIn;
struct bPdfPageCat;

struct bPdf {
    static dictionary unrollDict(const std::string&);
    static std::vector<std::string> unrollArray(const std::string&);

    static std::string rollDict(dictionary&);
    static std::string rollArray(const std::vector<std::string>&);

    static std::string extractObject(std::istream&, size_t startPos = std::string::npos,
                                    bool ignoreStreams = false, bool trim = false);

    // Object-type functions. In general, they return position of the first character of the object
    // or -1 when nothing was found. If "end" is provided, position of the last character can be also
    // obtained (assigned to this variable). End remains untouched if function finds nothing.
    static size_t isRef(const std::string&, size_t& end);
    static size_t isRef(const std::string&); 

    // auxiliary functions:
    static std::string itoa(int);                   // int to std::string
    static std::string getline(std::istream&);       // accepts all newline conventions(\n,\r\n,\r)
                                                    // end of line characters are not discarded
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
    size_t resolveIndirect(const std::string&, size_t&);
    size_t resolveIndirect(const std::string&, size_t&, size_t&);

    std::string extractObject(size_t startPos = std::string::npos, bool ignoreStreams = false, bool trim = false);
    std::string extractStream(size_t length, size_t pos = 0);

    dictionary trailer;

    bPdfIn(const char*);
    ~bPdfIn();

  private:
    std::ifstream file;

    void loadXref(size_t&);
    void loadXrefCompressed(size_t&);
    void loadXrefUncompressed();

    std::vector<bPdfXrefSection> xrefSections;

  friend class bPdfStream;
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

class bPdfStream : public bPdfNode  {
   public:
     std::string read();
     std::string readRaw();

     bPdfStream(size_t&, bPdfIn*);
     size_t length() { return len; }

   private:
     size_t streamPosition;
     size_t len;
     int filter;

 friend class bPdfIn;
} ;

#include "bPdf-auxil.cpp" // itoa (integer to string)
#include "bPdf.cpp"
#include "bPdf-type.cpp"
#include "bPdf-extractObject.cpp"
#include "bPdf-extractStream.cpp"

#include "bPdfNode.cpp"
#include "bPdfPageCat.cpp"

#include "bPdfIn.cpp"
#include "bPdfIn-getObjPos.cpp"
#include "bPdfIn-resolveIndirect.cpp"
#include "bPdfIn-loadXref.cpp"
#include "bPdfIn-loadPages.cpp"

#include "bPdfStream.cpp"
#include "bPdfStream-read.cpp"

   #endif // BPDF_H
