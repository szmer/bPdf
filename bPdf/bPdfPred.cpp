#include "bPdf-structure.h"

std::string bPdfPred::png(std::string &odata, int columns, int bits) {
    std::vector<unsigned char> ndata;
    std::vector<unsigned char> lrow;       // last row of new values
    int dpnt = 0;           // in odata

    for(int i=0; i<columns; i++)    // fill with empty initial vals
        lrow.push_back('\0');

    while(dpnt < odata.size()) {
        switch((int)odata[dpnt]) {       // first data in each colums specifies predictor
             case 0:     // png none
                for(int i=1; i<(columns+1); i++)
                     ndata.push_back( odata[dpnt+i] );
                break;

             case 1:    // png sub
                for(int i=1; i<(columns+1); i++) {
                    if(ndata.size() == 0) {
                         ndata.push_back(odata[1]);
                         continue;
                    } // if
                    ndata.push_back( (unsigned char) (odata[dpnt+i] + ndata[ndata.size()-1]) ); 
                } // for
                break;

             case 2:    // png up
                for(int i=1; i<(columns+1); i++)
                    ndata.push_back( (odata[dpnt+i] + lrow[i-1]) ); 
                break;

             case 3:    // png average
                for(int i=1; i<(columns+1); i++) {
                    if(ndata.size() == 0) {
                         ndata.push_back(odata[1]);
                         continue;
                    } // if
                    ndata.push_back( (unsigned char) (odata[dpnt+i] + (lrow[i-1]+ndata[ndata.size()-1])/2) ); 
                } // for
                break;

             case 4:   // png peath
                throw "Peath PNG predictor is not supported yet.";

             default:
                throw "Unknown PNG predictor in decoded stream.";
        } // switch odata[dpnt]
        dpnt += columns+1;
        lrow = std::vector<unsigned char>(ndata.begin()+(ndata.size()-columns), ndata.end());
    } // while dpnt < odata.size()

    return std::string((const char*)&ndata[0], ndata.size());
}

std::string bPdfPred::tiff(std::string &odata, int colors, int bits) {
    throw "Tiff predictor is not supported yet.";
    return std::string();
}