#include "PDFWriter.h"
#include "PDFError.h"
#include "PDFPage.h"
#include <fmt/format.h>
#include <stdexcept>

namespace PDF {
    Writer::Writer(const std::string& filename) : filename(filename) {
        g_error.current_function = "Writer::Writer";
        pdf = HPDF_New(error_handler, &g_error);
        if (!pdf) {
            throw std::runtime_error("Error creating PDF");
        }
    }

    Writer::~Writer() {
        if (pdf) HPDF_SaveToFile(pdf, filename.c_str());
        HPDF_Free(pdf);
    }
    void Writer::enableCompression() {
        g_error.current_function = "Writer::enableCompression";
        HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
    }
    HPDF_Font Writer::getFont(const std::string &fontName) {
        if (PDF::Page::isFontExist(fontName)) return HPDF_GetFont(pdf, fontName.c_str(), nullptr);
        return nullptr;
    }
}
