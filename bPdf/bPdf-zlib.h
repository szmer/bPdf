#include "zlib.h"

struct bPdfZlib {
   size_t data_pnt;
   std::string *data;
   z_stream strm;
   int z_state;

   std::string decompress(const size_t&);

   bPdfZlib(std::string*);
   bPdfZlib();
} ;

// empty constructor, lets bPdfStream have instance of object without initialization
bPdfZlib::bPdfZlib() {
    data = NULL;
}

bPdfZlib::bPdfZlib(std::string *source) {
    data_pnt = 0;
    data = source;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    z_state = inflateInit(&strm);

    if(z_state != Z_OK)
        std::cout << "Warning: failed to initialize Zlib decompression" << std::endl;
}

std::string bPdfZlib::decompress(const size_t& chunk) {

    if(z_state != Z_OK) {
        std::cout << "Warning: requested ZLib stream exhausted or flawed." << std::endl;
        return std::string("");
    } 

    char * input, * dcmpr;
    // Pointers are declared earlier to ensure that data.length() wouldn't affect data.c_str()
    // (anyway, it shouldn't). 

    strm.avail_in = data->length()-data_pnt;
    strm.avail_out = chunk;
 
    input = &(const_cast<char*>  (data->c_str())[data_pnt]);
    dcmpr = new char[chunk];

    strm.next_in = (Bytef*)input;
    strm.next_out = (Bytef*)dcmpr;

    z_state = inflate(&strm, Z_NO_FLUSH);

    std::string dcmprStr;

    if(z_state == Z_OK || z_state == Z_STREAM_END) {
        data_pnt = data->length()-data_pnt-strm.avail_in;
        dcmprStr = std::string(dcmpr, chunk-strm.avail_out);
    }
    else 
        std::cout << "Warning: ZLib failure." << std::endl;

    delete[] dcmpr;

    return dcmprStr;
}
