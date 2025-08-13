#pragma once
#include <hpdf.h>
#include "PDFPage.h"
#include <string>
namespace PDF {
    enum BorderType {
        borderNone,
        borderSolid,
        borderDouble
    };

    class Writer {
        HPDF_Doc pdf;
        std::string filename;
        std::string error_message;
    public:
        Writer(const std::string& filename);
        ~Writer();
        void enableCompression();
        HPDF_Font getFont(const std::string &fontName);
        auto addPage(PageSizes size = PageSizes::A4, PageOrientation orientation = PageOrientation::Portrait) { return PDF::Page(pdf, size, orientation); }
    };

    class Document {
        Writer writer;

    public:
        Document(const std::string& filename) : writer(filename) {}
        auto AddPage(PageOrientation orientation = PageOrientation::Portrait, PageSizes size = PageSizes::A4) { return writer.addPage(size, orientation); }
        ~Document() {}
    };
}