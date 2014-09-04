#ifndef BPDF_H
#define BPDF_H

/* Contains library inclusions and classes' declarations: */
#include "bPdf-structure.h"

#include "bPdfZlib.cpp"
#include "bPdfPred.cpp"
#include "bPdfXrefSection.cpp"
#include "bPdf-auxil.cpp" // itoa (integer to string)
#include "bPdf.cpp"
#include "bPdf-type.cpp"
#include "bPdf-extractObject.cpp"

#include "bPdfNode.cpp"
#include "bPdfIn-getPage.cpp"

#include "bPdfIn.cpp"
#include "bPdfIn-getObjByNum.cpp"
#include "bPdfIn-getObjPos.cpp"
#include "bPdfIn-obj.cpp"
#include "bPdfIn-resolveIndirect.cpp"
#include "bPdfIn-loadXref.cpp"
#include "bPdfIn-loadPages.cpp"

#include "bPdfStream.cpp"
#include "bPdfStream-read.cpp"
#include "bPdfContent.cpp"

#endif // BPDF_H