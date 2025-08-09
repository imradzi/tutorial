#include "PDFPage.h"
#include "PDFError.h"
#include "PDFFont.h"
#include "PDFCell.h"
#include <fmt/format.h>
#include <stdexcept>
#include <iostream>
namespace PDF {
    Page::Page(HPDF_Doc pdf) : pdf(pdf), page(HPDF_AddPage(pdf)) {
        g_error.current_function = "PDFPage::PDFPage";
        if (!page) {
            throw std::runtime_error("Error adding page");
        }
        h = HPDF_Page_GetHeight(page);
        w = HPDF_Page_GetWidth(page);
        HPDF_Page_Concat(page, 1, 0, 0, -1, 0, h); // 1) Make (0,0) be top-left for all drawing
    }

    HPDF_REAL Page::getTextWidth(const std::string& text, const std::string& font, int size) const {
        if (!isFontExist(font)) {
            throw std::runtime_error(fmt::format("Error getting font: {}", font));
        }
        auto fontPtr = HPDF_GetFont(pdf, font.c_str(), nullptr);
        HPDF_Page_SetFontAndSize(page, fontPtr, size);
        return HPDF_Page_TextWidth(page, text.c_str());
    }
    HPDF_REAL Page::getTextWidth(const std::string& text, HPDF_Font fontPtr, HPDF_REAL size) const {
        HPDF_Page_SetFontAndSize(page, fontPtr, size);
        return HPDF_Page_TextWidth(page, text.c_str());
    }

    HPDF_Font Page::getFont(const std::string& font) const {
        if (isFontExist(font)) {
            return HPDF_GetFont(pdf, font.c_str(), nullptr);
        } 
        return HPDF_Page_GetCurrentFont(page);
    }

    HPDF_REAL Page::getTextHeight(const std::string& font, int size) const {
        if (!isFontExist(font)) {
            throw std::runtime_error(fmt::format("Error getting font: {}", font));
        }
        auto fontPtr = HPDF_GetFont(pdf, font.c_str(), nullptr);
        HPDF_REAL ascent = HPDF_Font_GetAscent(fontPtr);
        HPDF_REAL descent = HPDF_Font_GetDescent(fontPtr); // Note: This is negative!
        return (ascent - descent) * size / 1000.0;
    }

    HPDF_REAL Page::getTextHeight() const {
        auto fontPtr = HPDF_Page_GetCurrentFont(page);
        auto size = HPDF_Page_GetCurrentFontSize(page);
        HPDF_REAL ascent = HPDF_Font_GetAscent(fontPtr);
        HPDF_REAL descent = HPDF_Font_GetDescent(fontPtr); // Note: This is negative!
        return (ascent - descent) * size / 1000.0;
    }
    HPDF_REAL Page::getTextHeight(HPDF_Font fontPtr, HPDF_REAL size) const {
        HPDF_REAL ascent = HPDF_Font_GetAscent(fontPtr);
        HPDF_REAL descent = HPDF_Font_GetDescent(fontPtr); // Note: This is negative!
        return (ascent - descent) * size / 1000.0;
    }

    HPDF_REAL Page::getCellHeight(PDF::Cell cell) {
        auto fontPtr = cell.font == nullptr ? HPDF_Page_GetCurrentFont(page) : cell.font;
        auto fontSize = cell.fontSize <= 0 ? HPDF_Page_GetCurrentFontSize(page) : cell.fontSize;
        auto textHeight = getTextHeight(fontPtr, fontSize);
        auto height = cell.height;
        if (height <= 0) {
            height = textHeight;
        }
        return height + 2*cell.padding + 2*cell.border_thickness;
    }
    HPDF_REAL Page::getCellWidth(PDF::Cell cell) {
        auto fontPtr = cell.font == nullptr ? HPDF_Page_GetCurrentFont(page) : cell.font;
        auto fontSize = cell.fontSize <= 0 ? HPDF_Page_GetCurrentFontSize(page) : cell.fontSize;
        auto textWidth = getTextWidth(cell.text, fontPtr, fontSize);
        auto width = cell.width;
        if (width <= 0) {
            width = textWidth;
        }
        return width + 2*cell.padding + 2*cell.border_thickness;
    }

    std::tuple<HPDF_REAL, HPDF_REAL> Page::addCell(PDF::Cell cell, HPDF_REAL x, HPDF_REAL y) {

        auto fontPtr = cell.font == nullptr ? HPDF_Page_GetCurrentFont(page) : cell.font;
        auto fontSize = cell.fontSize <= 0 ? HPDF_Page_GetCurrentFontSize(page) : cell.fontSize;
        HPDF_Page_SetFontAndSize(page, fontPtr, fontSize);
        auto textHeight = getTextHeight(fontPtr, fontSize);
        auto textWidth = getTextWidth(cell.text, fontPtr, fontSize);

        auto width = cell.width;
        HPDF_REAL deltaWidth = 0;
        if (width <= 0) {
            width = getTextWidth(cell.text);
        } else {
            auto balanceWidth = width - textWidth;
            if (cell.horizontal_justify == PDF::Justification::center) {
                deltaWidth = balanceWidth / 2;
            } else if (cell.horizontal_justify == PDF::Justification::right) {
                deltaWidth = balanceWidth;
            }
            if (deltaWidth < 0) deltaWidth = 0;
        }

        width += cell.padding*2 + cell.margin*2 + cell.border_thickness*2;
        
        auto height = cell.height;
        HPDF_REAL deltaHeight = 0;
        if (height <= 0) {
            height = textHeight;
        } else {
            auto balanceHeight = height - textHeight;
            if (cell.vertical_justify == PDF::Justification::center) {
                deltaHeight = balanceHeight / 2;
            } else if (cell.vertical_justify == PDF::Justification::top) {
                deltaHeight = balanceHeight;
            }
            if (deltaHeight < 0) deltaHeight = 0;
        }
        height += cell.padding*2 + cell.margin*2 + cell.border_thickness*2;

        auto rect = PDF::Rect{x+cell.margin, y-height+cell.margin, width-cell.margin, height-cell.margin};

        HPDF_Page_SetRGBFill(page, cell.background_color.r, cell.background_color.g, cell.background_color.b);
        HPDF_Page_Rectangle(page, rect.x, rect.y, rect.width, rect.height);
        HPDF_Page_Fill(page);

        if (cell.border_thickness > 0) {
            HPDF_Page_SetLineWidth(page, cell.border_thickness);
            HPDF_Page_SetRGBStroke(page, cell.border_color.r, cell.border_color.g, cell.border_color.b);
            HPDF_Page_Rectangle(page, rect.x, rect.y, rect.width, rect.height);
            HPDF_Page_Stroke(page);
        }
       
        HPDF_Page_BeginText(page);
        HPDF_Page_SetRGBFill(page, cell.text_color.r, cell.text_color.g, cell.text_color.b);
        HPDF_Page_SetTextMatrix(page, 1, 0, 0, -1, 0, h);       
        PDF::Font f(pdf, fontPtr);

        rect.x += cell.padding+cell.border_thickness+deltaWidth;
        rect.y += cell.padding+cell.border_thickness+deltaHeight;
        rect.width -= cell.padding+cell.border_thickness;
        rect.height -= cell.padding+cell.border_thickness;

        auto [slen, wwidth] = f.measureText(cell.text, rect.width, fontSize);
        auto text = cell.text;
        if (text.size() > slen) {
            text = cell.text.substr(0, slen-3);
            text += "...";
        }
        HPDF_Page_TextOut(page, rect.x, rect.y, text.c_str());
        HPDF_Page_EndText(page);

        return {width + cell.border_thickness, height + cell.border_thickness};
        
    }

    std::tuple<HPDF_REAL, HPDF_REAL> Page::addConstrainedCell(PDF::Cell cell, HPDF_REAL x, HPDF_REAL y, HPDF_REAL boxWidth, HPDF_REAL boxHeight) {

        auto fontPtr = cell.font == nullptr ? HPDF_Page_GetCurrentFont(page) : cell.font;
        auto fontSize = cell.fontSize <= 0 ? HPDF_Page_GetCurrentFontSize(page) : cell.fontSize;
        HPDF_Page_SetFontAndSize(page, fontPtr, fontSize);
        auto textHeight = getTextHeight(fontPtr, fontSize);
        auto textWidth = getTextWidth(cell.text, fontPtr, fontSize);

        auto width = boxWidth > 0 ? boxWidth: cell.width;
        if (width <= 0) {
            width = textWidth;
        }

        width += cell.padding*2 + cell.margin*2 + cell.border_thickness*2;
        
        auto height = boxHeight > 0 ? boxHeight: cell.height;
        HPDF_REAL deltaHeight = 0;
        if (height <= 0) {
            height = textHeight;
        } else {
            auto balanceHeight = height - textHeight;
            if (cell.vertical_justify == PDF::Justification::center) {
                deltaHeight = balanceHeight / 2;
            } else if (cell.vertical_justify == PDF::Justification::top) {
                deltaHeight = balanceHeight;
            }
            if (deltaHeight < 0) deltaHeight = 0;
        }
        height += cell.padding*2 + cell.margin*2 + cell.border_thickness*2;

        auto rect = PDF::Rect{x+cell.margin, y-height+cell.margin, width-cell.margin, height-cell.margin};

        HPDF_Page_SetRGBFill(page, cell.background_color.r, cell.background_color.g, cell.background_color.b);
        HPDF_Page_Rectangle(page, rect.x, rect.y, rect.width, rect.height);
        HPDF_Page_Fill(page);

        if (cell.border_thickness > 0) {
            HPDF_Page_SetLineWidth(page, cell.border_thickness);
            HPDF_Page_SetRGBStroke(page, cell.border_color.r, cell.border_color.g, cell.border_color.b);
            HPDF_Page_Rectangle(page, rect.x, rect.y, rect.width, rect.height);
            HPDF_Page_Stroke(page);
        }

       
        HPDF_Page_BeginText(page);
        HPDF_Page_SetRGBFill(page, cell.text_color.r, cell.text_color.g, cell.text_color.b);
        HPDF_Page_SetTextMatrix(page, 1, 0, 0, -1, 0, h);
        
        rect.x += cell.padding+cell.border_thickness;
        rect.y += cell.padding+cell.border_thickness+deltaHeight;
        rect.width -= cell.padding+cell.border_thickness;
        rect.height -= cell.padding+cell.border_thickness;

        HPDF_REAL top = rect.y;
        HPDF_REAL left = rect.x;
        HPDF_REAL bottom = top + rect.height;
        HPDF_REAL right = left + rect.width;
        HPDF_TextAlignment align = HPDF_TALIGN_LEFT;
        HPDF_UINT len;
        HPDF_Page_TextRect(page, left, top, right, bottom, cell.text.c_str(), align, &len);
        HPDF_Page_EndText(page);

        return {width + cell.border_thickness, height + cell.border_thickness};
        
    }

    HPDF_REAL Page::addText(const std::string& text, const std::string& font, int size, int x, int y, bool isBordered, int height) {
        if (!isFontExist(font)) {
            throw std::runtime_error(fmt::format("Error getting font: {}", font));
        }
        HPDF_Page_BeginText(page);
        auto fontPtr = HPDF_GetFont(pdf, font.c_str(), nullptr);
        HPDF_Page_SetFontAndSize(page, fontPtr, size);
        auto textHeight = getTextHeight(fontPtr, size);
        if (height == 0) {
            height = textHeight;
        }
        HPDF_Page_SetTextMatrix(page, 1, 0, 0, -1, 0, h);
        HPDF_Page_TextOut(page, x, y, text.c_str());
        HPDF_Page_EndText(page);
        if (isBordered) {
            HPDF_Page_SetLineWidth(page, border_width);
            HPDF_Page_SetRGBStroke(page, border_color.r, border_color.g, border_color.b);
            HPDF_Page_Rectangle(page, x-border_padding, y-height-border_padding, getTextWidth(text, font, size)+2*border_padding, height+2*border_padding);
            HPDF_Page_Stroke(page);
        }
        return height;
    }

    std::unordered_set<std::string> PDF::Page::fonts = {
        "Courier",
        "Courier-Bold",
        "Courier-Oblique",
        "Courier-BoldOblique",
        "Helvetica",
        "Helvetica-Bold",
        "Helvetica-Oblique",
        "Helvetica-BoldOblique",
        "Times-Roman",
        "Times-Bold",
        "Times-Italic",
        "Times-BoldItalic",
        "Symbol",
        "ZapfDingbats"
    };

}