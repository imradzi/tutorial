#pragma once
#include <hpdf.h>
#include "PDFPage.h"

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
        auto addPage() { return PDF::Page(pdf); }
    };
    
}