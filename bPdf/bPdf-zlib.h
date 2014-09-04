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