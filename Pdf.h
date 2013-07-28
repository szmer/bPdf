#ifdef PDFIN_SHOW_FATAL
  #define PDFIN_FATAL(x) std::cout << x// show program terminating errors
#else
  #define PDFIN_FATAL(x)
#endif

#ifdef PDFIN_SHOW_NOTICE
  #define PDFIN_NOTICE(x) std::cout << x// show notices
#else
  #define PDFIN_FATAL(x)
#endif

#include <iostream>
#include <cstdlib> // atoi() in many places
#include <cmath> // pow() in PdfIn::getXrefPos() 
#include <fstream>
#include <map>
#include <vector>
#include <algorithm> // reversing vector in PdfIn::loadPages(), comparing maps in PdfOut::add()
#include <sstream> // int to string in PdfOut::copy()

struct Pdf;
class PdfIn;
class PdfOut;
class PdfNode;
class PdfPage;
class PdfPageCat;

#ifndef PDF_H
#define PDF_H

typedef std::map<std::string, std::string> dictionary;

struct Pdf {
    static std::map<std::string, std::string> unrollDict(std::string);
    static std::vector<std::string> unrollArray(std::string&);
    static int entry(const std::string&, int&, std::string&, std::string, std::string);

    static std::string rollDict(std::map<std::string, std::string>&);
    static std::string rollArray(std::vector<std::string>&);

    static std::string extractObject(std::istream &source, bool ignoreStreams = false,
				size_t startPos = -1, bool trim = false);

    static size_t strfind(const char*, const char*, const int pos = 0);
    static std::string itoa(int);
} ;

class PdfNode : public Pdf {
  public:
    PdfIn* source;
    PdfOut* outstream;

    std::string get(std::string);
    void set(std::string, std::string);
    void join(std::string, std::string);

  private:
    std::map<std::string, std::string> dictionary;
    std::string reference; // in source document
  friend class PdfOut;
  friend class PdfStream;
  friend class PdfPage;
} ;

class PdfIn : public Pdf {
  public:
    std::ifstream file;

    // initialization methods
    void mapDocument();
    PdfPageCat loadPages();

    // PDF objects methods
    unsigned int resolveIndirect(const std::string&);
    unsigned int resolveIndirect(const std::string&, int&, int&);
    std::string extractObject(bool ignoreStreams = false, size_t startPos = -1, bool trim = false);

    void peekObj(unsigned int); // dbg
    PdfIn(const char*); // construct

  private:
    bool CR; // CR = Carriage Return (\r), new line style (\n or Win \r\n)
    std::map<std::string, std::string> trailer;
    std::vector <unsigned int> xrefData;

    // initialization methods:
    void checkCR();
    unsigned int getXrefPos();
    void getXrefData(unsigned int); 
    void getTrailer();
 
    struct PdfInPages { // used temporarily by loadPages()
	std::map<std::string, std::string> attributes;
	std::vector<std::string> kids;
	int attributesIndex; // index in vector PdfInPageCat::attributesDicts, if entry was made 
	PdfInPages(); // clears garbage in attributesIndex, sets it to -1
    } ;
} ;

class PdfOut : public Pdf {
  public:
    void add(PdfPage);
    void write(std::string); // ALWAYS use this to write something
    void insertRoot(std::map<std::string,std::string>);
    void insertRoot(PdfPageCat);
    void finish();
    PdfOut(const char*); //construct

    std::string copyResources(PdfIn&, size_t);
    std::string copyResources(PdfIn&, dictionary);
    std::string copyResources(PdfIn&, std::string);
    int writeIndirectObject(std::string);

   private:
    std::ofstream file;
    unsigned int counter; // byte offset
    std::vector<PdfPage> pages;
    std::vector<PdfNode> pagesNodes;
    std::map<size_t, int> copiedObjects; // objects copied from PdfIn (for now only one)
					    // key is byteOffs in source, value is obj number
    std::vector<size_t> positions; // used to build xref section
    PdfNode root;

    friend class PdfNode;
} ;

class PdfStream : public PdfNode {
  public:
    PdfStream(std::map<std::string, std::string>, size_t, PdfIn*);
    std::string fetchWholeStream();

  private:
    size_t streamPnt;
} ;

class PdfPage : public PdfNode {
// has dictionary and get-set-join routines
  public:
    PdfPage(std::string, int, std::map<std::string, std::string>, PdfIn*);
					// for creating from PdfInPageCat::getPage()
    PdfStream nextContentStream(); // *unsafe*, always check with following:
    bool endOfStreams();

  private:
    std::map<std::string, std::string> inheritedDict; // replaced by 
	// number indicator when added to PdfOut:
    int inDictNum;
    int contentsPnt; // position in content dict for throwing
  friend class PdfOut;
} ;

class PdfPageCat {
  public:   
    PdfPage getPage(int);
    int count() { return pages.size(); }

  private:
    struct Entry { // only locates page in document, see also PdfPage
	std::string reference;
 int attributesDict;
    } ;
    PdfIn *doc;
    std::vector<Entry> pages;
    std::vector< std::map <std::string, std::string> > attributesDicts;
    std::map<std::string, std::string> root;
  friend class PdfIn;
  friend class PdfOut;
} ;

#include "Pdf.cpp"
#include "Pdf-auxil.cpp"
#include "brudnopis.cpp"
#include "brudnout.cpp"

#include "PdfIn.cpp"
#include "PdfInInit.cpp" // mapDocument() & document initialization methods
#include "PdfIn-LoadPages.cpp"

#include "PdfOut.cpp" // constructors
#include "PdfOut-Finish.cpp"

#include "PdfNode.cpp"
#include "PdfStream.cpp"
#include "PdfPage.cpp"
#include "PdfPageCat.cpp"
   #endif // PDF_H
