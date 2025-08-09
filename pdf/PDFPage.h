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
        HPDF_REAL border_width = 1.0;
        HPDF_REAL border_padding = 5.0;
        PDF::Color border_color = PDF::Color(0, 0, 0);
    public:
        Page(HPDF_Doc pdf);
        HPDF_Page operator()() {return page; }
        HPDF_REAL height() {return h; }
        HPDF_REAL width() {return w; }
        static bool isFontExist(const std::string &fontName) {
            if (fonts.find(fontName) == fonts.end()) false;
            return true;
        }

        void setBorderWidth(HPDF_REAL width) {border_width = width; }
        void setBorderColor(const PDF::Color &c) {border_color = c; }
        void setBorderPadding(HPDF_REAL padding) {border_padding = padding; }
        HPDF_Font getFont(const std::string& font) const;

        HPDF_REAL getCellHeight(PDF::Cell cell);
        HPDF_REAL getCellWidth(PDF::Cell cell);
        HPDF_REAL getTextHeight() const;
        HPDF_REAL getTextHeight(const std::string& font, int size) const;
        HPDF_REAL getTextHeight(HPDF_Font fontPtr, HPDF_REAL size) const;
        HPDF_REAL getTextWidth(const std::string& text, const std::string& font, int size) const;
        HPDF_REAL getTextWidth(const std::string& text, HPDF_Font fontPtr, HPDF_REAL size) const;
        HPDF_REAL getTextWidth(const std::string& text) const {return HPDF_Page_TextWidth(page, text.c_str());}

        std::tuple<HPDF_REAL, HPDF_REAL> addCell(PDF::Cell cell, HPDF_REAL x, HPDF_REAL y);
        std::tuple<HPDF_REAL, HPDF_REAL> addConstrainedCell(PDF::Cell cell, HPDF_REAL x, HPDF_REAL y, HPDF_REAL boxWidth=0, HPDF_REAL boxHeight=0);
        HPDF_REAL addText(const std::string& text, const std::string& font, int size, int x, int y, bool isBordered = false, int height = 0);
    };
}