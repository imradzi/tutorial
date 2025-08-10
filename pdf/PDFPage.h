#pragma once
#include <hpdf.h>
#include <string>
#include <unordered_set>
#include <tuple>
#include "PDFCell.h"
#include "PDFColor.h"
#include "PDFFont.h"

namespace PDF {
    class Page {
        static std::unordered_set<std::string> fonts;
        HPDF_Doc pdf;
        HPDF_Page page;
        HPDF_REAL h;
        HPDF_REAL w;
    public:
        Page(HPDF_Doc pdf);
        HPDF_Page operator()() {return page; }
        HPDF_REAL height() {return h; }
        HPDF_REAL width() {return w; }
        static bool isFontExist(const std::string &fontName) {
            if (fonts.find(fontName) == fonts.end()) false;
            return true;
        }

        HPDF_Font getFont(const std::string& font) const;

        HPDF_REAL getTextHeight() const;
        HPDF_REAL getTextHeight(const std::string& font, HPDF_REAL size) const;
        HPDF_REAL getTextHeight(HPDF_Font fontPtr, HPDF_REAL size) const;
        HPDF_REAL getTextWidth(const std::string& text, const std::string& font, HPDF_REAL size) const;
        HPDF_REAL getTextWidth(const std::string& text, HPDF_Font fontPtr, HPDF_REAL size) const;
        HPDF_REAL getTextWidth(const std::string& text) const {return HPDF_Page_TextWidth(page, text.c_str());}

        HPDF_REAL computeCellHeight(PDF::Cell cell);
        HPDF_REAL computeCellWidth(PDF::Cell cell);

        std::tuple<HPDF_REAL, HPDF_REAL> addCell(PDF::Cell cell, HPDF_REAL x, HPDF_REAL y);
        std::tuple<HPDF_REAL, HPDF_REAL> addConstrainedCell(PDF::Cell cell, HPDF_REAL x, HPDF_REAL y, HPDF_REAL boxWidth=0, HPDF_REAL boxHeight=0);

        std::tuple<HPDF_REAL, HPDF_REAL> addText(ClientRect rect, const std::string& text, const std::string& font, HPDF_REAL fontSize, Color backgroundColor, Color textColor);
        std::tuple<HPDF_REAL, HPDF_REAL> addText(ClientRect rect, const std::string& text, Color backgroundColor, TextProperties properties);

        HPDF_STATUS drawImage(ClientRect rect, const unsigned char *data, size_t size);
        HPDF_STATUS drawQR(ClientRect rect, const std::string &qrText, int scale, int qrBlockSize);
        void drawRectangle(ClientRect rect);
    };
}