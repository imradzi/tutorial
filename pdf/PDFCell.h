#pragma once
#include <hpdf.h>
#include <string>
#include "PDFColor.h"

namespace PDF {
    enum Justification {
        left,
        right,
        center,
        top,
        bottom
    };
    
    struct Rect {
        HPDF_REAL x;
        HPDF_REAL y;
        HPDF_REAL width;
        HPDF_REAL height;
    };

    struct Cell {
        HPDF_REAL border_thickness = 1.0;
        HPDF_REAL padding = 5.0;
        HPDF_REAL margin=0.0;
        HPDF_REAL width=0;
        HPDF_REAL height=0;
        PDF::Color border_color = PDF::BLACK;
        std::string text;
        HPDF_Font font;
        HPDF_REAL fontSize;
        PDF::Color background_color = PDF::LIGHT_GRAY;
        PDF::Color text_color = PDF::BLACK;
        PDF::Justification horizontal_justify = PDF::Justification::left;
        PDF::Justification vertical_justify = PDF::Justification::center;
    };
}