#pragma once
#include <hpdf.h>
#include <string>
#include "PDFColor.h"
#include <vector>
#include <fmt/format.h>

namespace PDF {
    class Page;
    enum Alignment {
        alignLeft,
        alignRight,
        alignCenter,
        alignTop,
        alignBottom
    };
    
    struct Coord {
        HPDF_REAL x;
        HPDF_REAL y;
        std::string getString() const { return fmt::format("({},{})", x, y); }
        Coord setY(HPDF_REAL n) { y = n; return *this; } // set y and return a copy;
        Coord setX(HPDF_REAL n) { x = n; return *this; } // set x and return a copy;
        Coord moveBy(HPDF_REAL dx, HPDF_REAL dy) { x += dx; y += dy; return *this; }
    };

    struct Size {
        HPDF_REAL width;
        HPDF_REAL height;
        std::string getString() const { return fmt::format("w:{} h:{}", width, height); }
    };

    struct Border {
        HPDF_REAL size = 0.0;
        PDF::Color color = PDF::WHITE;

        std::string getString() const { return fmt::format("size:{} color:{}", size, color.getString()); }
    };

    struct TextProperties {
        HPDF_Font font = nullptr;
        HPDF_REAL fontSize = 0;
        PDF::Color color = PDF::BLACK;
        PDF::Alignment horizontalAlignment = PDF::Alignment::alignLeft;
        PDF::Alignment verticalAlignment = PDF::Alignment::alignCenter;
        HPDF_REAL getTextHeight(PDF::Page* page) const;
    };

    enum Position : int {   
        top = 0,
        right = 1,
        bottom = 2,
        left = 3
    };

    struct Rect {
        Coord topLeft;
        Coord bottomRight;
        HPDF_REAL getWidth() const { return bottomRight.x - topLeft.x; }
        HPDF_REAL getHeight() const { return bottomRight.y - topLeft.y; }
        void setWidth(HPDF_REAL width) { bottomRight.x = topLeft.x + width; }
        void setHeight(HPDF_REAL height) { bottomRight.y = topLeft.y + height; }
        void setSize(Size size) { setWidth(size.width); setHeight(size.height); }
        Size getSize() const { return Size {getWidth(), getHeight()}; }
        std::string getString() const { return fmt::format("topLeft:{} bottomRight:{}", topLeft.getString(), bottomRight.getString()); }
        Rect& moveTo(Coord coord);
        Rect& moveBy(Coord coord);
        Rect& shrinkAndMove(HPDF_REAL top, HPDF_REAL right, HPDF_REAL bottom, HPDF_REAL left);
    };

    inline std::string getString(const std::vector<Border> &borders) {
        std::string result;
        for (const auto &border : borders) {
            result += fmt::format("{} ", border.getString());
        }
        return result;
    }

    struct ClientRect {
        HPDF_REAL getBoundingSize(Position position) const {
            return borders[position].size + paddings[position].size + margins[position].size;
        }
        ClientRect& moveTo(Coord coord) { moveBy({ .x = coord.x - rect.topLeft.x , .y = coord.y - rect.topLeft.y }); }
        ClientRect& moveBy(Coord coord);
        Rect getOuterRect() const { return rect; } // all including borders, paddings, margins
        Rect getBorderRect() const { return borderRect; }
        Rect getPaddingRect() const { return paddingRect; }
        Rect getInnerRect() const { return innerRect; }
        std::string getString() const { return fmt::format("rect:{} margins:{} borders:{} paddings:{} backgroundColor:{}", rect.getString(), PDF::getString(margins), PDF::getString(borders), PDF::getString(paddings), backgroundColor.getString()); }

        Rect rect, borderRect, paddingRect, innerRect;
        std::vector<Border> margins{{},{},{},{}}; //top, right, bottom, left
        std::vector<Border> borders{{},{},{},{}}; //top, right, bottom, left
        std::vector<Border> paddings{{},{},{},{}}; //top, right, bottom, left
        PDF::Color backgroundColor = PDF::LIGHT_GRAY;

        static ClientRect&& init(ClientRect &&rect);
    };

    struct Cell {
        ClientRect rect;
        std::string text;
        TextProperties properties;
        std::string getString() const { return fmt::format("rect:{} text:{}", rect.getString(), text); }

    };
}