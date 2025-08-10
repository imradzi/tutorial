#pragma once
#include <hpdf.h>
#include "PDFPage.h"
#include <string>
namespace PDF {
    class Writer {
        HPDF_Doc pdf;
        std::string filename;
        std::string error_message;
    public:
        Writer(const std::string& filename);
        ~Writer();
        void enableCompression();
        HPDF_Font getFont(const std::string &fontName);
        auto addPage(HPDF_PageSizes size = HPDF_PAGE_SIZE_A4, HPDF_PageDirection orientation = HPDF_PAGE_PORTRAIT) { return PDF::Page(pdf, size, orientation); }
    };
    
}