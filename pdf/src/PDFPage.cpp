#include "../include/PDFPage.h"
#include "../include/PDFError.h"
#include "../include/PDFFont.h"
#include "../include/PDFCell.h"
#include "../include/qrgen.h"
#include <fmt/format.h>
#include <stdexcept>
#include <iostream>
namespace PDF {
    Page::Page(HPDF_Doc pdf, PageSizes size, PageOrientation orientation, CoordinateSystem coordinateSystem) : pdf(pdf), page(HPDF_AddPage(pdf)) {
        g_error.current_function = "PDFPage::PDFPage";
        if (!page) {
            throw std::runtime_error("Error adding page");
        }
        if (HPDF_Page_SetSize(page, (HPDF_PageSizes)size, (HPDF_PageDirection) orientation) != HPDF_OK) {
            throw std::runtime_error("Error setting page size");
        }
        ;
        pageRect = ClientRect::init({
            .rect = {
                .topLeft = {0, 0},
                .bottomRight = {HPDF_Page_GetWidth(page), HPDF_Page_GetHeight(page)}
            },
            .margins {{.size=40}, {.size=40}, {.size=40}, {.size=40}}
            //.borders {{.size = 1, .color = PDF::BLACK}, {.size = 1, .color = PDF::BLACK}, {.size = 1, .color = PDF::BLACK}, {.size = 1, .color = PDF::BLACK}},
            //.paddings {{.size = 40}, {.size = 40}, {.size = 40}, {.size = 40}}
        });
        if (coordinateSystem == CoordinateSystem::TOP_LEFT) {
            setTopLeftAsOrigin();
        } else {
            setBottomLeftAsOrigin();
        }
        drawRectangle(pageRect, 0, PDF::WHITE);
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

    HPDF_STATUS Page::setFont(const std::string& font, HPDF_REAL fontSize) const {
        if (!isFontExist(font)) {
            throw std::runtime_error(fmt::format("Error getting font: {}", font));
        }
        auto fontPtr = HPDF_GetFont(pdf, font.c_str(), nullptr);
        return HPDF_Page_SetFontAndSize(page, fontPtr, fontSize);
    }

    std::tuple<HPDF_Font, HPDF_REAL> Page::getCurrentFont() const {
        return { HPDF_Page_GetCurrentFont(page), HPDF_Page_GetCurrentFontSize(page) };
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

    HPDF_REAL Page::computeCellHeight(PDF::Cell cell) const {
        auto fontPtr = cell.properties.font == nullptr ? HPDF_Page_GetCurrentFont(page) : cell.properties.font;
        auto fontSize = cell.properties.fontSize <= 0 ? HPDF_Page_GetCurrentFontSize(page) : cell.properties.fontSize;
        auto textHeight = getTextHeight(fontPtr, fontSize);
        auto height = cell.rect.getOuterRect().getHeight();
        if (height <= 0) {
            height = textHeight+cell.rect.getBoundingSize(Position::top) + cell.rect.getBoundingSize(Position::bottom);
        }
        return height;
    }
    HPDF_REAL Page::computeCellWidth(PDF::Cell cell) const {
        auto fontPtr = cell.properties.font == nullptr ? HPDF_Page_GetCurrentFont(page) : cell.properties.font;
        auto fontSize = cell.properties.fontSize <= 0 ? HPDF_Page_GetCurrentFontSize(page) : cell.properties.fontSize;
        auto textWidth = getTextWidth(cell.text, fontPtr, fontSize);
        auto width = cell.rect.getOuterRect().getWidth();
        if (width <= 0) {
            width = textWidth+cell.rect.getBoundingSize(Position::left) + cell.rect.getBoundingSize(Position::right);
        }
        return width;
    }

    std::tuple<HPDF_REAL, HPDF_REAL> Page::addCell(PDF::Cell cell, HPDF_REAL x, HPDF_REAL y) const {
        TextProperties prop = cell.properties;
        if (prop.font == nullptr) prop.font = HPDF_Page_GetCurrentFont(page);
        if (prop.fontSize == 0) prop.fontSize = HPDF_Page_GetCurrentFontSize(page);
        HPDF_Page_SetFontAndSize(page, prop.font, prop.fontSize);
        ClientRect cellRect = cell.rect;
        cellRect.rect.moveTo({x, y});
        return addText(cellRect, cell.text, prop, cell.rect.backgroundColor );
    }

    std::tuple<HPDF_REAL, HPDF_REAL> Page::addConstrainedCell(PDF::Cell cell, HPDF_REAL x, HPDF_REAL y, HPDF_REAL boxWidth, HPDF_REAL boxHeight) const {
        TextProperties prop = cell.properties;
        if (prop.font == nullptr) prop.font = HPDF_Page_GetCurrentFont(page);
        if (prop.fontSize == 0) prop.fontSize = HPDF_Page_GetCurrentFontSize(page);
        HPDF_Page_SetFontAndSize(page, prop.font, prop.fontSize);
        auto textHeight = getTextHeight(prop.font, prop.fontSize);
        auto textWidth = getTextWidth(cell.text, prop.font, prop.fontSize);

        auto width = boxWidth > 0 ? boxWidth: cell.rect.getInnerRect().getWidth();
        if (width <= 0) {
            width = textWidth;
        }

        width += cell.rect.getBoundingSize(Position::left) + cell.rect.getBoundingSize(Position::right);
        
        auto height = boxHeight > 0 ? boxHeight: cell.rect.getInnerRect().getHeight();
        HPDF_REAL deltaHeight = 0;
        if (height <= textHeight) {
            height = textHeight;
        } else {
            auto balanceHeight = height - textHeight;
            if (prop.verticalAlignment == PDF::Alignment::alignCenter) {
                deltaHeight = balanceHeight / 2;    
            } else if (prop.verticalAlignment == PDF::Alignment::alignTop) {
                deltaHeight = balanceHeight;
            }
            if (deltaHeight < 0) deltaHeight = 0;
        }
        height += cell.rect.getBoundingSize(Position::top) + cell.rect.getBoundingSize(Position::bottom);

        cell.rect.rect.setSize(Size {width, height});
        cell.rect.rect.moveTo(Coord{x, y});

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
        HPDF_Page_SetRGBFill(page, prop.color.r, prop.color.g, prop.color.b);
        HPDF_Page_SetTextMatrix(page, 1, 0, 0, -1, 0, HPDF_Page_GetHeight(page));
        
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

    void Page::writeText(Coord position, const std::string& text) {
        PDF::Color color = PDF::BLACK;
        HPDF_Page_BeginText(page);
        HPDF_Page_SetRGBFill(page, color.r, color.g, color.b);
        HPDF_Page_SetTextMatrix(page, 1, 0, 0, -1, 0, HPDF_Page_GetHeight(page));
        HPDF_Page_TextOut(page, position.x, position.y, text.c_str());
        HPDF_Page_EndText(page);
    }

    std::tuple<HPDF_REAL, HPDF_REAL> Page::addText(ClientRect outerRect, const std::string& text, TextProperties prop, Color backgroundColor) const {
        auto textHeight = getTextHeight(prop.font, prop.fontSize);
        auto textWidth = getTextWidth(text, prop.font, prop.fontSize);

        auto width = outerRect.getInnerRect().getWidth();
        HPDF_REAL deltaWidth = 0;
        if (width <= 0) {
            width = textWidth;
        } else if (width > textWidth) {
            auto balanceWidth = width - textWidth;
            if (prop.horizontalAlignment == PDF::Alignment::alignCenter) {
                deltaWidth = balanceWidth / 2;
            } else if (prop.horizontalAlignment == PDF::Alignment::alignRight) {
                deltaWidth = balanceWidth;
            }
            if (deltaWidth < 0) deltaWidth = 0;
        }

        width += outerRect.getBoundingSize(Position::left) + outerRect.getBoundingSize(Position::right);
        
        auto height = outerRect.getInnerRect().getHeight();
        HPDF_REAL deltaHeight = 0;
        if (height <= textHeight) {
            height = textHeight;
        } else {
            auto balanceHeight = height - textHeight;
            if (prop.verticalAlignment == PDF::Alignment::alignCenter) {
                deltaHeight = balanceHeight / 2;
            } else if (prop.verticalAlignment == PDF::Alignment::alignTop) {
                deltaHeight = balanceHeight;
            }
            if (deltaHeight < 0) deltaHeight = 0;
        }
        height += outerRect.getBoundingSize(Position::top) + outerRect.getBoundingSize(Position::bottom);

        outerRect.rect.setSize(Size{width, height});
        fillWithColor(outerRect.getOuterRect(), outerRect.margins[Position::top].color);

        if (outerRect.borders[Position::top].size > 0) {
            auto rect = outerRect.getBorderRect();
            drawRectangle(rect, outerRect.borders[Position::top].size, outerRect.borders[Position::top].color);
        }

        fillWithColor(outerRect.getBorderRect(), outerRect.paddings[Position::top].color);

        // innermost rect where text is going to be drawn.
        auto innerRect = outerRect.getInnerRect();
        fillWithColor(innerRect, backgroundColor);

        drawText(innerRect, deltaWidth, deltaHeight, text, prop.font, prop.fontSize, prop.color);
        return {outerRect.getOuterRect().getWidth(), outerRect.getOuterRect().getHeight()}; // the size of all.
    }

    std::tuple<HPDF_REAL, HPDF_REAL> Page::addText(ClientRect outerRect, const std::string& text, const std::string& font, HPDF_REAL fontSize, Color textColor, Color backgroundColor) const {
        if (!isFontExist(font)) {
            throw std::runtime_error(fmt::format("Error getting font: {}", font));
        }

        auto fontPtr = HPDF_GetFont(pdf, font.c_str(), nullptr);
        HPDF_Page_SetFontAndSize(page, fontPtr, fontSize);
        return addText(outerRect, text, TextProperties {.font = fontPtr, .fontSize = fontSize, .color = textColor}, backgroundColor);
    }

     std::tuple<HPDF_REAL, HPDF_REAL> Page::addText(ClientRect outerRect, const std::string& text, Color textColor, Color backgroundColor) const {
         auto fontPtr = HPDF_Page_GetCurrentFont(page);
         auto fontSize = HPDF_Page_GetCurrentFontSize(page);
         return addText(outerRect, text, TextProperties {.font = fontPtr, .fontSize = fontSize, .color = textColor}, backgroundColor);
     }

     void Page::fillWithColor(Rect rect, Color color) const {
         HPDF_Page_SetRGBFill(page, color.r, color.g, color.b);
         HPDF_Page_Rectangle(page, rect.topLeft.x, rect.topLeft.y, rect.getWidth(), rect.getHeight());
         HPDF_Page_Fill(page);
     }

     HPDF_STATUS Page::drawText(Rect rect, HPDF_REAL deltaWidth, HPDF_REAL deltaHeight, const std::string& text, HPDF_Font font, HPDF_REAL fontSize, Color color) const {
         HPDF_Page_BeginText(page);
         HPDF_Page_SetRGBFill(page, color.r, color.g, color.b);
         HPDF_Page_SetTextMatrix(page, 1, 0, 0, -1, 0, HPDF_Page_GetHeight(page));
         PDF::Font f(pdf, font);
         auto [slen, wwidth] = f.measureText(text, rect.getWidth(), fontSize);
         std::string str = text;
         if (str.size() > slen) {
             str = text.substr(0, slen - 3);
             str += "...";
         }
         HPDF_Page_TextOut(page, rect.topLeft.x + deltaWidth, rect.topLeft.y + deltaHeight, str.c_str());
         return HPDF_Page_EndText(page);
     }

     HPDF_STATUS Page::drawRectangle(Rect rect, HPDF_REAL size, Color color) const {
         HPDF_Page_SetLineWidth(page, size);
         HPDF_Page_SetRGBStroke(page, color.r, color.g, color.b);
         HPDF_Page_Rectangle(page, rect.topLeft.x, rect.topLeft.y, rect.getWidth(), rect.getHeight());
         return HPDF_Page_Stroke(page);
     }

    HPDF_STATUS Page::drawWidget(ClientRect outerRect, Color backgroundColor, std::function<HPDF_STATUS(Rect &)> draw) const {
        auto& innerRect = outerRect.innerRect;
        //auto width = innerRect.getWidth() + outerRect.getBoundingSize(Position::left) + outerRect.getBoundingSize(Position::right);
        //auto height = innerRect.getHeight() + outerRect.getBoundingSize(Position::top) + outerRect.getBoundingSize(Position::bottom);
        //outerRect.rect.setSize(Size {width, height});
        if (outerRect.margins[Position::top].size > 0) fillWithColor(outerRect.getOuterRect(), outerRect.margins[Position::top].color);
        if (outerRect.borders[Position::top].size > 0) drawRectangle(outerRect.getBorderRect(), outerRect.borders[Position::top].size, outerRect.borders[Position::top].color);
        if (outerRect.paddings[Position::top].size > 0) fillWithColor(outerRect.getBorderRect(), outerRect.paddings[Position::top].color);

        // innermost rect where text is going to be drawn.
        fillWithColor(innerRect, backgroundColor);
        return draw(innerRect);
    }

    HPDF_STATUS Page::drawImage(Rect innerRect, const unsigned char *data, size_t size) const {
        auto image = HPDF_LoadPngImageFromMem(pdf, data, (HPDF_UINT)size);
        return HPDF_Page_DrawImage(page, image, innerRect.topLeft.x, innerRect.topLeft.y, innerRect.getWidth(), innerRect.getHeight());
    }

    HPDF_STATUS Page::drawQR(Rect rect, const std::string &qrText, int scale, int border) const {
        unsigned char *pngBuf = NULL;
        size_t pngSize = 0;
        auto [w, h] = qrcode_text_to_png_mem(qrText.c_str(), scale, border, &pngBuf, &pngSize, BLOCK_STYLE_ROUNDED); // dimension is in pixels
        if (w > 0 && h > 0) {
            auto rc = drawImage(rect, pngBuf, pngSize); // draw image will draw pixel in points. Depends on translation matrix.
            free(pngBuf);
            return rc;
        }
        return HPDF_FILE_IO_ERROR;
    }

    HPDF_STATUS Page::drawText(ClientRect outerRect, const std::string& text, TextProperties prop, Color backgroundColor) const {
        auto textHeight = getTextHeight(prop.font, prop.fontSize);
        auto textWidth = getTextWidth(text, prop.font, prop.fontSize);

        auto width = outerRect.getInnerRect().getWidth();
        HPDF_REAL deltaWidth = 0;
        if (width <= 0) {
            width = textWidth;
        } else {
            auto balanceWidth = width - textWidth;
            if (prop.horizontalAlignment == PDF::Alignment::alignCenter) {
                deltaWidth = balanceWidth / 2;
            } else if (prop.horizontalAlignment == PDF::Alignment::alignRight) {
                deltaWidth = balanceWidth;
            }
            if (deltaWidth < 0) deltaWidth = 0;
        }

        auto height = outerRect.getInnerRect().getHeight();
        HPDF_REAL deltaHeight = 0;
        HPDF_REAL extraHeight = 0;
        if (height <= 0) {
            height = textHeight;
            if (prop.verticalAlignment == PDF::Alignment::alignCenter) extraHeight = -height / 2;
        } else {
            auto balanceHeight = height - textHeight;
            if (prop.verticalAlignment == PDF::Alignment::alignCenter) {
                deltaHeight = balanceHeight / 2;
                extraHeight = -textHeight/2;
            } else if (prop.verticalAlignment == PDF::Alignment::alignTop) {
                deltaHeight = balanceHeight;
                extraHeight = -textHeight;
            }
            if (deltaHeight < 0) deltaHeight = 0;
        }
        deltaHeight += extraHeight;
        return drawWidget(outerRect, backgroundColor, [this, text, prop, &deltaWidth, &deltaHeight](Rect innerRect) {
            return drawText(innerRect, deltaWidth, deltaHeight, text, prop.font, prop.fontSize, prop.color);
        });
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