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

typedef std::map<std::string, std::string> dictionary;
