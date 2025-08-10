#include "PDFPage.h"
#include "PDFError.h"
#include "PDFFont.h"
#include "PDFCell.h"
#include "qrgen.h"
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

    HPDF_REAL Page::getTextWidth(const std::string& text, const std::string& font, HPDF_REAL size) const {
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

    HPDF_REAL Page::getTextHeight(const std::string& font, HPDF_REAL size) const {
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

    HPDF_REAL Page::computeCellHeight(PDF::Cell cell) {
        auto fontPtr = cell.properties.font == nullptr ? HPDF_Page_GetCurrentFont(page) : cell.properties.font;
        auto fontSize = cell.properties.fontSize <= 0 ? HPDF_Page_GetCurrentFontSize(page) : cell.properties.fontSize;
        auto textHeight = getTextHeight(fontPtr, fontSize);
        auto height = cell.rect.getOuterRect().getHeight();
        if (height <= 0) {
            height = textHeight+cell.rect.getBoundingSize(Position::top) + cell.rect.getBoundingSize(Position::bottom);
        }
        return height;
    }
    HPDF_REAL Page::computeCellWidth(PDF::Cell cell) {
        auto fontPtr = cell.properties.font == nullptr ? HPDF_Page_GetCurrentFont(page) : cell.properties.font;
        auto fontSize = cell.properties.fontSize <= 0 ? HPDF_Page_GetCurrentFontSize(page) : cell.properties.fontSize;
        auto textWidth = getTextWidth(cell.text, fontPtr, fontSize);
        auto width = cell.rect.getOuterRect().getWidth();
        if (width <= 0) {
            width = textWidth+cell.rect.getBoundingSize(Position::left) + cell.rect.getBoundingSize(Position::right);
        }
        return width;
    }

    std::tuple<HPDF_REAL, HPDF_REAL> Page::addCell(PDF::Cell cell, HPDF_REAL x, HPDF_REAL y) {
        auto fontPtr = cell.properties.font == nullptr ? HPDF_Page_GetCurrentFont(page) : cell.properties.font;
        auto fontSize = cell.properties.fontSize <= 0 ? HPDF_Page_GetCurrentFontSize(page) : cell.properties.fontSize;
        HPDF_Page_SetFontAndSize(page, fontPtr, fontSize);
        return addText(cell.rect, cell.text, cell.rect.backgroundColor, TextProperties{.font=fontPtr, .fontSize=fontSize, .color=cell.properties.color});
    }

    std::tuple<HPDF_REAL, HPDF_REAL> Page::addConstrainedCell(PDF::Cell cell, HPDF_REAL x, HPDF_REAL y, HPDF_REAL boxWidth, HPDF_REAL boxHeight) {

        auto fontPtr = cell.properties.font == nullptr ? HPDF_Page_GetCurrentFont(page) : cell.properties.font;
        auto fontSize = cell.properties.fontSize <= 0 ? HPDF_Page_GetCurrentFontSize(page) : cell.properties.fontSize;
        HPDF_Page_SetFontAndSize(page, fontPtr, fontSize);
        auto textHeight = getTextHeight(fontPtr, fontSize);
        auto textWidth = getTextWidth(cell.text, fontPtr, fontSize);

        auto width = boxWidth > 0 ? boxWidth: cell.rect.getInnerRect().getWidth();
        if (width <= 0) {
            width = textWidth;
        }

        width += cell.rect.getBoundingSize(Position::left) + cell.rect.getBoundingSize(Position::right);
        
        auto height = boxHeight > 0 ? boxHeight: cell.rect.getInnerRect().getHeight();
        HPDF_REAL deltaHeight = 0;
        if (height <= 0) {
            height = textHeight;
        } else {
            auto balanceHeight = height - textHeight;
            if (cell.properties.verticalJustify == PDF::Justification::justifyCenter) {
                deltaHeight = balanceHeight / 2;    
            } else if (cell.properties.verticalJustify == PDF::Justification::justifyTop) {
                deltaHeight = balanceHeight;
            }
            if (deltaHeight < 0) deltaHeight = 0;
        }
        height += cell.rect.getBoundingSize(Position::top) + cell.rect.getBoundingSize(Position::bottom);

        cell.rect.rect.setSize(Size{width, height});

        auto rect = cell.rect.getOuterRect();

        HPDF_Page_SetRGBFill(page, cell.rect.backgroundColor.r, cell.rect.backgroundColor.g, cell.rect.backgroundColor.b);
        HPDF_Page_Rectangle(page, rect.topLeft.x, rect.topLeft.y, rect.getWidth(), rect.getHeight());
        HPDF_Page_Fill(page);

        if (cell.rect.borders[Position::top].size > 0) {
            rect = cell.rect.getBorderRect();
            HPDF_Page_SetLineWidth(page, cell.rect.borders[Position::top].size);
            HPDF_Page_SetRGBStroke(page, cell.rect.borders[Position::top].color.r, cell.rect.borders[Position::top].color.g, cell.rect.borders[Position::top].color.b);    
            HPDF_Page_Rectangle(page, rect.topLeft.x, rect.topLeft.y, rect.getWidth(), rect.getHeight());
            HPDF_Page_Stroke(page);
        }

       
        HPDF_Page_BeginText(page);
        HPDF_Page_SetRGBFill(page, cell.properties.color.r, cell.properties.color.g, cell.properties.color.b);
        HPDF_Page_SetTextMatrix(page, 1, 0, 0, -1, 0, h);
        
        rect = cell.rect.getInnerRect();

        HPDF_REAL top = rect.topLeft.y;
        HPDF_REAL left = rect.topLeft.x;
        HPDF_REAL bottom = rect.bottomRight.y;
        HPDF_REAL right = rect.bottomRight.x;
        HPDF_TextAlignment align = HPDF_TALIGN_LEFT;
        HPDF_UINT len;
        HPDF_Page_TextRect(page, left, top, right, bottom, cell.text.c_str(), align, &len);
        HPDF_Page_EndText(page);

        return {cell.rect.getOuterRect().getWidth(), cell.rect.getOuterRect().getHeight()};
        
    }

    std::tuple<HPDF_REAL, HPDF_REAL> Page::addText(ClientRect outerRect, const std::string& text, Color backgroundColor, TextProperties prop) {
        auto textHeight = getTextHeight(prop.font, prop.fontSize);
        auto textWidth = getTextWidth(text, prop.font, prop.fontSize);

        auto width = outerRect.getInnerRect().getWidth();
        HPDF_REAL deltaWidth = 0;
        if (width <= 0) {
            width = textWidth;
        } else {
            auto balanceWidth = width - textWidth;
            if (prop.horizontalJustify == PDF::Justification::justifyCenter) {
                deltaWidth = balanceWidth / 2;
            } else if (prop.horizontalJustify == PDF::Justification::justifyRight) {
                deltaWidth = balanceWidth;
            }
            if (deltaWidth < 0) deltaWidth = 0;
        }

        width += outerRect.getBoundingSize(Position::left) + outerRect.getBoundingSize(Position::right);
        
        auto height = outerRect.getInnerRect().getHeight();
        HPDF_REAL deltaHeight = 0;
        if (height <= 0) {
            height = textHeight;
        } else {
            auto balanceHeight = height - textHeight;
            if (prop.verticalJustify == PDF::Justification::justifyCenter) {
                deltaHeight = balanceHeight / 2;
            } else if (prop.verticalJustify == PDF::Justification::justifyTop) {
                deltaHeight = balanceHeight;
            }
            if (deltaHeight < 0) deltaHeight = 0;
        }
        height += outerRect.getBoundingSize(Position::top) + outerRect.getBoundingSize(Position::bottom);

        outerRect.rect.setSize(Size{width, height});
        auto rect = outerRect.getOuterRect();
        HPDF_Page_SetRGBFill(page, backgroundColor.r, backgroundColor.g, backgroundColor.b);
        HPDF_Page_Rectangle(page, rect.topLeft.x, rect.topLeft.y, rect.getWidth(), rect.getHeight());
        HPDF_Page_Fill(page);

        if (outerRect.borders[Position::top].size > 0) {
            rect = outerRect.getBorderRect();
            HPDF_Page_SetLineWidth(page, outerRect.borders[Position::top].size);
            HPDF_Page_SetRGBStroke(page, outerRect.borders[Position::top].color.r, outerRect.borders[Position::top].color.g, outerRect.borders[Position::top].color.b);
            HPDF_Page_Rectangle(page, rect.topLeft.x, rect.topLeft.y, rect.getWidth(), rect.getHeight());
            HPDF_Page_Stroke(page);
        }
       
        HPDF_Page_BeginText(page);
        HPDF_Page_SetRGBFill(page, prop.color.r, prop.color.g, prop.color.b);
        HPDF_Page_SetTextMatrix(page, 1, 0, 0, -1, 0, h);       
        PDF::Font f(pdf, prop.font);

        auto innerRect = outerRect.getInnerRect();

        auto [slen, wwidth] = f.measureText(text, innerRect.getWidth(), prop.fontSize);
        auto str = text;
        if (str.size() > slen) {
            str = text.substr(0, slen-3);
            str += "...";
        }
        HPDF_Page_TextOut(page, innerRect.topLeft.x, innerRect.topLeft.y, str.c_str());
        HPDF_Page_EndText(page);
        return {outerRect.getOuterRect().getWidth(), outerRect.getOuterRect().getHeight()};
    }

    std::tuple<HPDF_REAL, HPDF_REAL> Page::addText(ClientRect outerRect, const std::string& text, const std::string& font, HPDF_REAL fontSize, Color backgroundColor, Color textColor) {
        if (!isFontExist(font)) {
            throw std::runtime_error(fmt::format("Error getting font: {}", font));
        }

        auto fontPtr = HPDF_GetFont(pdf, font.c_str(), nullptr);
        HPDF_Page_SetFontAndSize(page, fontPtr, fontSize);
        return addText(outerRect, text, backgroundColor, TextProperties{.font=fontPtr, .fontSize=fontSize, .color=textColor});
    }

    // std::tuple<HPDF_REAL, HPDF_REAL> Page::addText(ClientRect outerRect, const std::string& text, Color backgroundColor, Color textColor) {
    //     auto fontPtr = HPDF_Page_GetCurrentFont(page);
    //     auto fontSize = HPDF_Page_GetCurrentFontSize(page);
    //     return addText(outerRect, text, backgroundColor, TextProperties{.font=fontPtr, .fontSize=fontSize, .color=textColor});
    // }

    void Page::drawRectangle(ClientRect rect) {
        HPDF_Page_SetRGBFill(page, rect.backgroundColor.r, rect.backgroundColor.g, rect.backgroundColor.b);
        HPDF_Page_Rectangle(page, rect.getOuterRect().topLeft.x, rect.getOuterRect().topLeft.y, rect.getOuterRect().getWidth(), rect.getOuterRect().getHeight());
        HPDF_Page_Fill(page);

        if (rect.borders[Position::top].size > 0) {
            auto innerRect = rect.getBorderRect();
            HPDF_Page_SetLineWidth(page, rect.borders[Position::top].size);
            HPDF_Page_SetRGBStroke(page, rect.borders[Position::top].color.r, rect.borders[Position::top].color.g, rect.borders[Position::top].color.b);
            HPDF_Page_Rectangle(page, innerRect.topLeft.x, innerRect.topLeft.y, innerRect.getWidth(), innerRect.getHeight());    
            HPDF_Page_Stroke(page);
        }
    }

    HPDF_STATUS Page::drawImage(ClientRect rect, const unsigned char *data, size_t size) {

        HPDF_Page_SetRGBFill(page, rect.backgroundColor.r, rect.backgroundColor.g, rect.backgroundColor.b);
        HPDF_Page_Rectangle(page, rect.getOuterRect().topLeft.x, rect.getOuterRect().topLeft.y, rect.getOuterRect().getWidth(), rect.getOuterRect().getHeight());
        HPDF_Page_Fill(page);

        if (rect.borders[Position::top].size > 0) {
            auto innerRect = rect.getBorderRect();
            HPDF_Page_SetLineWidth(page, rect.borders[Position::top].size);
            HPDF_Page_SetRGBStroke(page, rect.borders[Position::top].color.r, rect.borders[Position::top].color.g, rect.borders[Position::top].color.b);
            HPDF_Page_Rectangle(page, innerRect.topLeft.x, innerRect.topLeft.y, innerRect.getWidth(), innerRect.getHeight());    
            HPDF_Page_Stroke(page);
        }

        auto image = HPDF_LoadPngImageFromMem(pdf, data, (HPDF_UINT)size);
        auto innerRect = rect.getInnerRect();
        return HPDF_Page_DrawImage(page, image, innerRect.topLeft.x, innerRect.topLeft.y, innerRect.getWidth(), innerRect.getHeight());
    }

    HPDF_STATUS Page::drawQR(ClientRect rect, const std::string &qrText, int scale, int border) {
        unsigned char *pngBuf = NULL;
        size_t pngSize = 0;
        if (qrcode_text_to_png_mem(qrText.c_str(), scale, border, &pngBuf, &pngSize, BLOCK_STYLE_ROUNDED)) {
            auto rc = drawImage(rect, pngBuf, pngSize);
            free(pngBuf);
            return rc;
        }
        return HPDF_FILE_IO_ERROR;
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