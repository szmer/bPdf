#ifdef PDFPAGE_SHOW_FATAL
  #define PDFPAGE_FATAL(x) std::cout << x// show program terminating errors
#endif

#ifdef PDFPAGE_SHOW_NOTICE
  #define PDFPAGE_NOTICE(x) std::cout << x// show notices
#endif

#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include "PdfIn.h"

#pragma once
#ifndef PDFPAGE_H
#define PDFPAGE_H

#include "PdfPage.cpp"
#endif
