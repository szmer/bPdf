#include "bPdf/bPdf.h"

#include <iostream>

using namespace std;

int main(int argc, char **argv) {

    std::string filename;
    if(argc > 1)
        filename = argv[1];
    else
        filename = "doc.pdf";

    try {
	bPdfIn input(filename.c_str());
	bPdfPageCat pageCat = input.loadPages();
	std::cout << "Document contains " << input.trailer["/Size"] << " objects"
		<< " and " << pageCat.count() << " pages.\n";
    }
    catch(const char* msg) {
	std::cout << "Fatal error: " << msg << '\n';
    }

    return 0;
}
