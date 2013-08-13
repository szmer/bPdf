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
#include "bPdf-zlib.h"
#include "bPdf-predictors.h"

class bPdfIn;
struct bPdfPageCat;
class bPdfStream;

struct bPdf {
    static dictionary unrollDict(const std::string&);
    static std::vector<std::string> unrollArray(const std::string&);

    static std::string rollDict(dictionary&);
    static std::string rollArray(const std::vector<std::string>&);

    static std::string extractObject(std::istream&, size_t startPos = std::string::npos,
                                     bool trim = false, bool ignoreStreams = true);

    // Type-checking functions. In general, they return position of the first character of the object
    // or -1 when nothing was found.
    // If *end* is provided, position of the last character can be also obtained (is assigned to
    // this variable). *End* remains untouched if function finds nothing.
    // If *retry* is set to false, each functions checks only if object of desired type occurs in the
    // beginning of string (starting from 0 index). In contrary, when *retry* is set to true, function 
    // retries to find an object until its 1st instance is found or end of string is reached.

    static size_t isRef(const std::string&, size_t& end, bool retry = true);
    static size_t isRef(const std::string&, bool retry = false);

    // Type-checking funcs for containers assign -1 to *end* if they found container, but it seems to
    // end outside given string.
    static size_t isArr(const std::string&, size_t& end, bool retry = true);
    static size_t isArr(const std::string&, bool retry = false);
    static size_t isDict(const std::string&, size_t& end, bool retry = true);
    static size_t isDict(const std::string&, bool retry = false);

    // Auxiliary functions.
    static std::string itoa(int);                   // int to std::string
    static std::string getline(std::istream&);       // accepts all PDF newline conventions(\n,\r\n,\r)
                                                    // end of line characters are not discarded
} ;

struct bPdfNode {
    bPdfIn* source;

    std::string get(std::string);
    void set(std::string, std::string);
    void join(std::string, std::string);

    dictionary dict;
    int objNum;
} ;

struct bPdfPage : public bPdfNode {
// has dictionary and get-set-join functions
    int inherDictNum;
} ;

class bPdfIn {
  public:
    // initialization methods
    bPdfPageCat loadPages();

                                     // PDF objects methods: *common methods*. //
    // _getObjByNum_: finds in document indirect object of given number (taken usually from resolveIndirect() call)
    // and returns it as string.
    // *trim*: shall we trim delimeters? ("<number> <number> obj" and "endobj" keywords)
    // *ignoreStreams*: shall we ignore stream content? If it is set to false and function encounters "stream" keyword,
    // it returns data exactly to this place, excluding the keyword. Otherwise it reads WHOLE object, to the "endobj" 
    // keyword. Most often it's silly and very inefficient, use bPdfStream class instead.
    std::string getObjByNum(int, bool trim = false, bool ignoreStreams = true); 

    // _resolveIndirect_: invokes type-checking method isRef() on given string. If successful, returns object
    // number, otherwise -1.
    // To *pos* may be assigned position of the last character of found reference, to *beg* - the first character.
    // These variables remain untouched if function fails to find a reference.
    // *retry* value indicates if function should only try to find a reference placed in the beginning of the string.
    int resolveIndirect(const std::string&, bool retry = false);
    int resolveIndirect(const std::string&, size_t& pos, bool retry = true);
    int resolveIndirect(const std::string&, size_t& pos, size_t& beg, bool retry = true);

                                    // PDF objects methods: *underlying methods*// 
    size_t getObjPos(int);
    std::string extractObject(size_t startPos = std::string::npos, bool trim = false, bool ignoreStreams = true);
    std::string extractObjFromStream(bPdfStream, int num);

    dictionary trailer;

    bPdfIn(const char*);
    ~bPdfIn();

  private:
    std::ifstream file;

    dictionary loadXref(size_t);
    dictionary loadXrefCompressed(size_t);
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
     std::string readsome(size_t amount = -1);
     std::string read();
     std::string readsomeRaw(size_t amount = -1);
     std::string readRaw();

     bPdfStream(size_t&, bPdfIn*);
     size_t length() { return len; }
     bPdfStream() { streamPointer=0; }

   private:
     size_t streamPosition;
     size_t streamPointer;
     size_t len;
     int filter;
     std::string unconsumed;

     bPdfZlib zlib;

 friend class bPdfIn;
} ;

#include "bPdf-intern.cpp"
#include "bPdf-auxil.cpp" // itoa (integer to string)
#include "bPdf.cpp"
#include "bPdf-type.cpp"
#include "bPdf-extractObject.cpp"

#include "bPdfNode.cpp"
#include "bPdfPageCat.cpp"

#include "bPdfIn.cpp"
#include "bPdfIn-getObjByNum.cpp"
#include "bPdfIn-getObjPos.cpp"
#include "bPdfIn-resolveIndirect.cpp"
#include "bPdfIn-loadXref.cpp"
#include "bPdfIn-loadPages.cpp"

#include "bPdfStream.cpp"
#include "bPdfStream-read.cpp"

   #endif // BPDF_H
