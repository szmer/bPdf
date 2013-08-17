// Stream filters.
#define BPDF_FILTER_ASCIIHEX      1
#define BPDF_FILTER_ASCII85       2
#define BPDF_FILTER_LZW           3
#define BPDF_FILTER_FLATE         4
#define BPDF_FILTER_RUNLENGTH     5
#define BPDF_FILTER_CCITTFAX      6
#define BPDF_FILTER_JBIG2         7
#define BPDF_FILTER_DCT           8
#define BPDF_FILTER_JPX           9
#define BPDF_FILTER_CRYPT        10
#define BPDF_FILTER_UNKNOWN     254
#define BPDF_FILTER_NONE        255    // no filters, raw content

// Amounts of data to be read by some functions (in bytes).
#ifndef BPDF_ZLIB_CHUNK
#define BPDF_ZLIB_CHUNK      262144    // 256 kb (bPdfStream::readsome)
#endif

typedef std::vector<std::string> array;
typedef std::map<std::string, std::string> dictionary;
