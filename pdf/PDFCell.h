#pragma once
#include <hpdf.h>
#include <string>
#include "PDFColor.h"
#include <vector>
namespace PDF {
    enum Justification {
        justifyLeft,
        justifyRight,
        justifyCenter,
        justifyTop,
        justifyBottom
    };
    
    struct Coord {
        HPDF_REAL x;
        HPDF_REAL y;
    };

    struct Size {
        HPDF_REAL width;
        HPDF_REAL height;
    };

    struct Border {
        HPDF_REAL size = 0.0;
        PDF::Color color = PDF::WHITE;
    };

    struct TextProperties {
        HPDF_Font font = nullptr;
        HPDF_REAL fontSize = 0;
        PDF::Color color = PDF::BLACK;
        PDF::Justification horizontalJustify = PDF::Justification::justifyLeft;
        PDF::Justification verticalJustify = PDF::Justification::justifyCenter;
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
        Size getSize() const { return Size{getWidth(), getHeight()}; }
        
    };

    struct ClientRect {
        Rect rect;
        std::vector<Border> borders{{},{},{},{}}; //top, right, bottom, left
        std::vector<Border> paddings{{},{},{},{}}; //top, right, bottom, left
        std::vector<Border> margins{{},{},{},{}}; //top, right, bottom, left
        PDF::Color backgroundColor = PDF::LIGHT_GRAY;
        HPDF_REAL getBoundingSize(Position position) const {
            return borders[position].size + paddings[position].size + margins[position].size;
        }
        Rect getOuterRect() { return rect; } // all including borders, paddings, margins
        Rect getBorderRect() const { // with margins - where print border rect is.
            Rect borderRect;
            borderRect.topLeft.x = rect.topLeft.x + margins[left].size;
            borderRect.topLeft.y = rect.topLeft.y + margins[top].size;
            borderRect.bottomRight.x = rect.bottomRight.x -  margins[right].size;
            borderRect.bottomRight.y = rect.bottomRight.y - margins[bottom].size;
            return borderRect;
        }
        Rect getInnerRect() const {
            Rect innerRect;
            innerRect.topLeft.x = rect.topLeft.x + paddings[left].size + borders[left].size + margins[left].size;
            innerRect.topLeft.y = rect.topLeft.y + paddings[top].size + borders[top].size + margins[top].size;
            innerRect.bottomRight.x = rect.bottomRight.x - paddings[right].size - borders[right].size - margins[right].size;
            innerRect.bottomRight.y = rect.bottomRight.y - paddings[bottom].size -borders[bottom].size - margins[bottom].size;
            return innerRect;
        }
    };

    struct Cell {
        ClientRect rect;
        std::string text;
        TextProperties properties;
    };
}